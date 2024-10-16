const char *__get_os_type(r_bin_le_obj_t *bin) {
	switch (bin->header->os) {
	case 1: return "OS/2";
	case 2: return "Windows";
	case 3: return "DOS 4.x";
	case 4: return "Windows 386";
	case 5: return "IBM Microkernel Personality Neutral";
	default: return "Unknown";
	}
}