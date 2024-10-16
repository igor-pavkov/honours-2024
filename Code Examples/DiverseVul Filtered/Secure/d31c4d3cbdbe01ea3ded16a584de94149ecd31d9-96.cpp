R_API int r_bin_use_arch(RBin *bin, const char *arch, int bits,
			  const char *name) {
	RBinFile *binfile = r_bin_file_find_by_arch_bits (bin, arch, bits, name);
	RBinObject *obj = NULL;
	if (binfile) {
		obj = r_bin_object_find_by_arch_bits (binfile, arch, bits, name);
		if (!obj) {
			if (binfile->xtr_data) {
				RBinXtrData *xtr_data = r_list_get_n (binfile->xtr_data, 0);
				if (!r_bin_file_object_new_from_xtr_data (bin, binfile,
						UT64_MAX, r_bin_get_laddr (bin), xtr_data)) {
					return false;
				}
				obj = r_list_get_n (binfile->objs, 0);
			}
		}
	} else {
		void *plugin = r_bin_get_binplugin_by_name (bin, name);
		if (plugin) {
			if (bin->cur) {
				bin->cur->curplugin = plugin;
			}
			binfile = r_bin_file_new (bin, "-", NULL, 0, 0, 0, 999, NULL, NULL, false);
			// create object and set arch/bits
			obj = r_bin_object_new (binfile, plugin, 0, 0, 0, 1024);
			binfile->o = obj;
			obj->info = R_NEW0 (RBinInfo);
			obj->info->arch = strdup (arch);
			obj->info->bits = bits;
		}
	}
	return (binfile && r_bin_file_set_cur_binfile_obj (bin, binfile, obj));
}