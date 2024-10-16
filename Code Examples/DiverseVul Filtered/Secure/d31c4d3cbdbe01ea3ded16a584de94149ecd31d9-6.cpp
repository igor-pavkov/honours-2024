R_API RBinObject *r_bin_object_find_by_arch_bits(RBinFile *binfile,
						  const char *arch, int bits,
						  const char *name) {
	RBinObject *obj = NULL;
	RListIter *iter = NULL;
	RBinInfo *info = NULL;
	r_list_foreach (binfile->objs, iter, obj) {
		info = obj->info;
		if (info && info->arch && info->file &&
		   (bits == info->bits) &&
			!strcmp (info->arch, arch) &&
			!strcmp (info->file, name)) {
			break;
		}
		obj = NULL;
	}
	return obj;
}