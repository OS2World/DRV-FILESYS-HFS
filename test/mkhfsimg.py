#!/usr/bin/env python3
# HFS/2 - A Hierarchical File System Driver for OS/2
# GPL v2 or later.  Written 2026.
#
# mkhfsimg.py - create a small HFS (Macintosh) floppy image with a few files and
# folders, for testing the HFS.IFS driver.
#
#   python mkhfsimg.py [output.img]        (default: hfs-test.img, 1440 KB)
#
# Layout: 512-byte sectors, no partition map.  Sectors 0-1 boot blocks, 2 MDB,
# 3 volume bitmap, 4.. allocation blocks of 512 bytes (extents tree, catalog tree,
# file data), second-to-last sector the alternate MDB.  The volume is written
# from the public HFS on-disk format description; after writing, the image is
# read back with an independent small parser as a self-check.

import struct, sys, time

SECTORS = 2880
BLK = 512
VOLNAME = "HFS2 Test"
EPOCH_1904 = 2082844800


def mac_now():
    return int(time.time()) + EPOCH_1904


def pstr(s, width=None):
    b = bytes([len(s)]) + s.encode("mac_roman")
    return b if width is None else b.ljust(width, b"\0")


def sortkey(name):
    return name.upper().encode("mac_roman")



# Standard OS/2 (FAT-style) 1.44 MB boot sector header, as written by PREPARE.EXE:
# OS/2 cannot use a diskette without a recognisable boot sector (this is what the
# filter driver HFSFLT.FLT fakes at run time).  System ID is "HFS     ".
BOOT_HDR = bytes([
    0xEB, 0x3C, 0x90, 0x49, 0x42, 0x4D, 0x20, 0x32, 0x30, 0x2E, 0x30, 0x00, 0x02, 0x01, 0x01, 0x00,
    0x02, 0xE0, 0x00, 0x40, 0x0B, 0xF0, 0x09, 0x00, 0x12, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00, 0x00])


def boot_sector(label, serial=0x48465332):
    s = bytearray(BLK)
    s[0:len(BOOT_HDR)] = BOOT_HDR
    s[39:43] = struct.pack("<I", serial)
    s[43:54] = label.encode("mac_roman")[:11].ljust(11, b" ")
    s[54:62] = b"HFS     "
    s[510] = 0x55
    s[511] = 0xAA
    return bytes(s)


class Tree:
    """one B-tree node of 512 bytes"""
    NODE = 512

    @staticmethod
    def node(kind, height, records, flink=0, blink=0):
        body = bytearray()
        offs = []
        pos = 14
        for r in records:
            offs.append(pos)
            body += r
            pos += len(r)
        offs.append(pos)                       # free space offset
        n = bytearray(Tree.NODE)
        struct.pack_into(">IIbbHH", n, 0, flink, blink, kind, height, len(records), 0)
        n[14:14 + len(body)] = body
        end = Tree.NODE
        for o in offs:
            end -= 2
            struct.pack_into(">H", n, end, o)
        assert pos <= end, "node overflow"
        return bytes(n)

    @staticmethod
    def header(depth, root, nrecs, first, last, nodesize, maxkey, nnodes, nfree, usedmap):
        hdr = struct.pack(">HIIIIHHII", depth, root, nrecs, first, last, nodesize, maxkey, nnodes, nfree)
        hdr += bytes(106 - len(hdr))
        user = bytes(128)
        mapbytes = bytearray(256)
        for i in usedmap:
            mapbytes[i // 8] |= 0x80 >> (i % 8)
        n = bytearray(Tree.NODE)
        struct.pack_into(">IIbbHH", n, 0, 0, 0, 1, 0, 3, 0)
        n[14:14 + 106] = hdr
        n[120:120 + 128] = user
        n[248:248 + 256] = mapbytes
        for idx, o in enumerate((14, 120, 248, 504)):
            struct.pack_into(">H", n, Tree.NODE - 2 * (idx + 1), o)
        return bytes(n)


def cat_key(parent, name):
    k = bytes([0]) + struct.pack(">I", parent) + pstr(name)
    k = bytes([len(k)]) + k
    if len(k) % 2:
        k += b"\0"
    return k


def dir_record(cnid, valence, now):
    return struct.pack(">bbHHIIII16s16s16s", 1, 0, 0, valence, cnid, now, now, 0, bytes(16), bytes(16), bytes(16))


def file_record(cnid, ftype, creator, start, length, now):
    phys = ((length + BLK - 1) // BLK) * BLK
    nblk = phys // BLK
    finfo = ftype.encode("ascii") + creator.encode("ascii") + bytes(8)
    ext = struct.pack(">HH", start, nblk) + bytes(8) if nblk else bytes(12)
    return (struct.pack(">bbbb16sIHIIHIIIII16sH", 2, 0, 0x02, 0, finfo, cnid, start, length, phys, 0, 0, 0,
                        now, now, 0, bytes(16), 0) + ext + bytes(12) + bytes(4))


def thread_record(kind, parent, name):
    return struct.pack(">bb8sI", kind, 0, bytes(8), parent) + pstr(name, 32)


def build():
    now = mac_now()
    # ---- content -------------------------------------------------------------
    readme = b"This is a test volume for HFS.IFS (HFS/2).\r"
    hello = b"Hello from inside a folder.\r"
    big = bytes((i * 7 + 3) & 0xFF for i in range(5000))
    files = [  # (parent, name, type, creator, data)
        (2, "ReadMe.txt", "TEXT", "ttxt", readme),
        (16, "hello.txt", "TEXT", "ttxt", hello),
        (2, "Big file", "BINA", "????", big),
    ]
    # ---- allocation plan -----------------------------------------------------
    nalblks = (SECTORS - 2 - 4)              # sectors 4 .. SECTORS-3
    xt_blocks, ct_blocks = 4, 8
    nextblk = 0
    xt_start = nextblk; nextblk += xt_blocks
    ct_start = nextblk; nextblk += ct_blocks
    placed = []
    for parent, name, t, c, data in files:
        blocks = (len(data) + BLK - 1) // BLK
        placed.append((parent, name, t, c, data, nextblk))
        nextblk += blocks
    used = nextblk
    # ---- catalog records -----------------------------------------------------
    cnid_file = {}
    nxt = 17
    recs = []   # (parent, sortname, bytes)
    # root dir and its thread
    recs.append((1, VOLNAME, cat_key(1, VOLNAME), dir_record(2, 2, now)))
    recs.append((2, "", cat_key(2, ""), thread_record(3, 1, VOLNAME)))
    # folder "Docs" (CNID 16)
    recs.append((2, "Docs", cat_key(2, "Docs"), dir_record(16, 1, now)))
    recs.append((16, "", cat_key(16, ""), thread_record(3, 2, "Docs")))
    for parent, name, t, c, data, start in placed:
        cnid = nxt; nxt += 1
        recs.append((parent, name, cat_key(parent, name), file_record(cnid, t, c, start, len(data), now)))
        recs.append((cnid, "", cat_key(cnid, ""), thread_record(4, parent, name)))
    recs.sort(key=lambda r: (r[0], sortkey(r[1])))
    # split the sorted records over two leaf nodes (one 512-byte node is too small)
    full = [r[2] + r[3] for r in recs]
    cap = 512 - 14 - 2          # room for records + the free-space offset entry
    first, used_sz = [], 0
    for i, rec in enumerate(full):
        if used_sz + len(rec) + 2 > cap - 2 * len(first):
            break
        first.append(rec); used_sz += len(rec)
    second = full[len(first):]
    assert first and second, "expected two leaves"
    leaf1 = Tree.node(-1, 1, first, flink=3, blink=0)
    leaf2 = Tree.node(-1, 1, second, flink=0, blink=2)
    firstkeys = [recs[0][2], recs[len(first)][2]]
    index = Tree.node(0, 2, [firstkeys[0] + struct.pack(">I", 2), firstkeys[1] + struct.pack(">I", 3)])
    cat = (Tree.header(2, 1, len(recs), 2, 3, 512, 37, ct_blocks, ct_blocks - 4, [0, 1, 2, 3]) + index
           + leaf1 + leaf2 + bytes(512 * (ct_blocks - 4)))
    ext = Tree.header(0, 0, 0, 0, 0, 512, 7, xt_blocks, xt_blocks - 1, [0]) + bytes(512 * (xt_blocks - 1))
    # ---- volume bitmap -------------------------------------------------------
    bitmap = bytearray(BLK)
    for i in range(used):
        bitmap[i // 8] |= 0x80 >> (i % 8)
    # ---- MDB -------------------------------------------------------------------
    nfiles_root = sum(1 for p, *_ in placed if p == 2)
    mdb = bytearray(BLK)
    # fill field by field (clear and explicit)
    struct.pack_into(">H", mdb, 0, 0x4244)
    struct.pack_into(">I", mdb, 2, now)
    struct.pack_into(">I", mdb, 6, now)
    struct.pack_into(">H", mdb, 10, 0x0100)
    struct.pack_into(">H", mdb, 12, nfiles_root)
    struct.pack_into(">H", mdb, 14, 3)               # drVBMSt
    struct.pack_into(">H", mdb, 16, used)            # drAllocPtr
    struct.pack_into(">H", mdb, 18, nalblks)
    struct.pack_into(">I", mdb, 20, BLK)
    struct.pack_into(">I", mdb, 24, 2048)            # drClpSiz
    struct.pack_into(">H", mdb, 28, 4)               # drAlBlSt
    struct.pack_into(">I", mdb, 30, nxt)             # drNxtCNID
    struct.pack_into(">H", mdb, 34, nalblks - used)  # drFreeBks
    mdb[36:36 + 28] = pstr(VOLNAME, 28)
    struct.pack_into(">I", mdb, 74, 2048)            # drXTClpSiz
    struct.pack_into(">I", mdb, 78, 4096)            # drCTClpSiz
    struct.pack_into(">H", mdb, 82, 1)               # drNmRtDirs
    struct.pack_into(">I", mdb, 84, len(placed))     # drFilCnt
    struct.pack_into(">I", mdb, 88, 1)               # drDirCnt (folders besides root)
    struct.pack_into(">I", mdb, 130, xt_blocks * BLK)
    struct.pack_into(">HH", mdb, 134, xt_start, xt_blocks)
    struct.pack_into(">I", mdb, 146, ct_blocks * BLK)
    struct.pack_into(">HH", mdb, 150, ct_start, ct_blocks)
    # ---- assemble the image --------------------------------------------------
    img = bytearray(SECTORS * BLK)
    img[0:BLK] = boot_sector(VOLNAME, int(time.time()) & 0xFFFFFFFF)
    img[2 * BLK:3 * BLK] = mdb
    img[3 * BLK:4 * BLK] = bitmap
    def put(block, data):
        o = (4 + block) * BLK
        img[o:o + len(data)] = data
    put(xt_start, ext)
    put(ct_start, cat)
    for parent, name, t, c, data, start in placed:
        put(start, data)
    img[(SECTORS - 2) * BLK:(SECTORS - 1) * BLK] = mdb        # alternate MDB
    return bytes(img), files


def selfcheck(img, files):
    """independent read-back: MDB, bitmap, catalog walk, file contents"""
    mdb = img[2 * BLK:2 * BLK + 162]
    assert struct.unpack(">H", mdb[0:2])[0] == 0x4244
    nal = struct.unpack(">H", mdb[18:20])[0]
    ctstart, ctlen = struct.unpack(">HH", mdb[150:154])
    cat = img[(4 + ctstart) * BLK:(4 + ctstart + ctlen) * BLK]
    hdr = cat[14:14 + 106]
    depth, root, nrecs, firstleaf = struct.unpack(">HIII", hdr[0:14])
    found, total, nd = {}, 0, firstleaf
    while nd:
        node = cat[nd * 512:(nd + 1) * 512]
        flink, blink, kind, height, n = struct.unpack(">IIbbH", node[0:12])
        assert kind == -1 and height == 1
        total += n
        for i in range(n):
            off = struct.unpack(">H", node[512 - 2 * (i + 1):512 - 2 * i])[0]
            klen = node[off]
            parent = struct.unpack(">I", node[off + 2:off + 6])[0]
            nlen = node[off + 6]
            name = node[off + 7:off + 7 + nlen].decode("mac_roman")
            rec = off + 1 + klen
            if rec % 2: rec += 1
            if node[rec] == 2:
                start = struct.unpack(">H", node[rec + 24:rec + 26])[0]
                length = struct.unpack(">I", node[rec + 26:rec + 30])[0]
                found[(parent, name)] = img[(4 + start) * BLK:(4 + start) * BLK + length]
        nd = flink
    assert total == nrecs, (total, nrecs)
    n = total
    for parent, name, t, c, data in files:
        assert found[(parent, name)] == data, name
    return nal, n


if __name__ == "__main__":
    out = sys.argv[1] if len(sys.argv) > 1 else "hfs-test.img"
    img, files = build()
    nal, n = selfcheck(img, files)
    open(out, "wb").write(img)
    print("wrote %s (%d bytes): volume '%s', %d allocation blocks, %d catalog records, %d files verified"
          % (out, len(img), VOLNAME, nal, n, len(files)))
