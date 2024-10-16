const char *__get_module_type(r_bin_le_obj_t *bin) {
	switch (bin->header->mflags & M_TYPE_MASK) {
	case M_TYPE_EXE: return "Program module (EXE)";
	case M_TYPE_DLL: return "Library module (DLL)";
	case M_TYPE_PDD: return "Physical Device Driver";
	case M_TYPE_VDD: return "Virtual Device Driver";
	default: return "Unknown";
	}
}