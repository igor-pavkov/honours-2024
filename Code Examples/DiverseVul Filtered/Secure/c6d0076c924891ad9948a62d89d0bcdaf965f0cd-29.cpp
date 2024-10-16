ut64 Elf_(r_bin_elf_get_fini_offset)(ELFOBJ *bin) {
	ut64 entry = Elf_(r_bin_elf_get_entry_offset) (bin);
	ut8 buf[512];
	if (!bin) {
		return 0LL;
	}

	if (r_buf_read_at (bin->b, entry+11, buf, sizeof (buf)) == -1) {
		bprintf ("Warning: read (get_fini)\n");
		return 0;
	}
	if (*buf == 0x68) { // push // x86/32 only
		ut64 addr;
		memmove (buf, buf+1, 4);
		addr = (ut64)r_read_le32 (buf);
		return Elf_(r_bin_elf_v2p) (bin, addr);
	}
	return 0;
}