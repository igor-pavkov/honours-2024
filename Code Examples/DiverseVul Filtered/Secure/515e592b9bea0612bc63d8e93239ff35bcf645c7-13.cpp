static bool load_buffer(RBinFile *bf, void **bin_obj, RBuffer *buf, ut64 loadaddr, Sdb *sdb) {
#if 0
	SYMBOLS HEADER

 0	MAGIC	02ff01ff
 4	VERSION 1 (little endian)
 8      ffffffff
16      002b0000 01000000 { 0x2b00, 0x0000 }
24	UUID    16 bytes
40	2621 d85b 2100 2000 0000 0000 0000 0000
56	ffff ffff ffff ff7f 0c00 0000 0900 0000
72	0400 0000 6800 0000 2f76 6172 2f66 6f6c .... 4, 104 /// 104 length string
184
0x000000b8  5f5f 5445 5854 0000 0000 0000 0000 0000 0000 0000 0000 0000 0080 0000 0000 0000  __TEXT..........................
0x000000d8  5f5f 4441 5441 0000 0000 0000 0000 0000 0080 0000 0000 0000 0040 0000 0000 0000  __DATA...................@......
0x000000f8  5f5f 4c4c 564d 0000 0000 0000 0000 0000 00c0 0000 0000 0000 0000 0100 0000 0000  __LLVM..........................
0x00000118  5f5f 4c49 4e4b 4544 4954 0000 0000 0000 00c0 0100 0000 0000 00c0 0000 0000 0000  __LINKEDIT......................

#endif
	// 0 - magic check, version ...
	SymbolsHeader sh = parseHeader (buf);
	if (!sh.valid) {
		eprintf ("Invalid headers\n");
		return false;
	}
	SymbolsMetadata sm = parseMetadata (buf, 0x40);
	char * file_name = NULL;
	if (sm.namelen) {
		file_name = calloc (sm.namelen + 1, 1);
		if (!file_name) {
			return false;
		}
		if (r_buf_read_at (buf, 0x50, (ut8*)file_name, sm.namelen) != sm.namelen) {
			return false;
		}
	}
	RCoreSymCacheElement *element = parseDragons (bf, buf, sm.addr + sm.size, sm.bits, file_name);
	if (element) {
		*bin_obj = element;
		return true;
	}
	free (file_name);
	return false;
}