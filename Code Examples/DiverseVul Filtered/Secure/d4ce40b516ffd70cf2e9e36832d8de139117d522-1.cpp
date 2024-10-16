static SymbolsMetadata parseMetadata(RBuffer *buf, int off) {
	SymbolsMetadata sm = {0};
	ut8 b[0x100] = {0};
	(void)r_buf_read_at (buf, off, b, sizeof (b));
	sm.addr = off;
	sm.cputype = r_read_le32 (b);
	sm.arch = typeString (sm.cputype, &sm.bits);
	//  eprintf ("0x%08x  cputype  0x%x -> %s\n", 0x40, sm.cputype, typeString (sm.cputype));
	// bits = (strstr (typeString (sm.cputype, &sm.bits), "64"))? 64: 32;
	sm.subtype = r_read_le32 (b + 4);
	sm.cpu = subtypeString (sm.subtype);
	//  eprintf ("0x%08x  subtype  0x%x -> %s\n", 0x44, sm.subtype, subtypeString (sm.subtype));
	sm.n_segments = r_read_le32 (b + 8);
	// int count = r_read_le32 (b + 0x48);
	sm.namelen = r_read_le32 (b + 0xc);
	// eprintf ("0x%08x  count    %d\n", 0x48, count);
	// eprintf ("0x%08x  strlen   %d\n", 0x4c, sm.namelen);
	// eprintf ("0x%08x  filename %s\n", 0x50, b + 16);
	int delta = 16;
	//sm.segments = parseSegments (buf, off + sm.namelen + delta, sm.n_segments);
	sm.size = (sm.n_segments * 32) + sm.namelen + delta;

	// hack to detect format
	ut32 nm, nm2, nm3;
	r_buf_read_at (buf, off + sm.size, (ut8 *)&nm, sizeof (nm));
	r_buf_read_at (buf, off + sm.size + 4, (ut8 *)&nm2, sizeof (nm2));
	r_buf_read_at (buf, off + sm.size + 8, (ut8 *)&nm3, sizeof (nm3));
	// eprintf ("0x%x next %x %x %x\n", off + sm.size, nm, nm2, nm3);
	if (r_read_le32 (&nm3) != 0xa1b22b1a) {
		sm.size -= 8;
		//		is64 = true;
	}
	return sm;
}