static int getoffset(RBinFile *arch, int type, int idx) {
	struct r_bin_dex_obj_t *dex = arch->o->bin_obj;
	switch (type) {
	case 'm': // methods
		// TODO: ADD CHECK
		return offset_of_method_idx (arch, dex, idx);
	case 'o': // objects
		break;
	case 's': // strings
		if (dex->header.strings_size > idx) {
			if (dex->strings) return dex->strings[idx];
		}
		break;
	case 't': // type
		return dex_get_type_offset (arch, idx);
	case 'c': // class
		return dex_get_type_offset (arch, idx);
		//return sdb_num_get (cdb, sdb_fmt (0, "%d", idx), 0);
	}
	return -1;
}