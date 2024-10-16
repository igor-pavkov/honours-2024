const char *__get_arch(r_bin_le_obj_t *bin) {
	switch (bin->header->cpu) {
	case 1:
	case 2:
	case 3:
		return "x86";
	case 0x20:
	case 0x21:
		return "i860";
	case 0x40:
	case 0x41:
	case 0x42:
		return "mips";
	default:
		return "Unknown";
	}
}