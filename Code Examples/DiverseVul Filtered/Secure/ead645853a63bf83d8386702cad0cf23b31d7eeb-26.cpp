static char *dex_type_descriptor(RBinDexObj *bin, int type_idx) {
	if (type_idx < 0 || type_idx >= bin->header.types_size) {
		return NULL;
	}
	return getstr (bin, bin->types[type_idx].descriptor_id);
}