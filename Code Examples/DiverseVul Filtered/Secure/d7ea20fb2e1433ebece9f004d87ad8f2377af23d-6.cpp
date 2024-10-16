RList *r_bin_le_get_imports(r_bin_le_obj_t *bin) {
	RList *l = r_list_newf ((RListFree)r_bin_import_free);
	if (!l) {
		return NULL;
	}
	LE_image_header *h = bin->header;
	ut64 offset = (ut64)h->impproc + bin->headerOff + 1; // First entry is a null string
	ut64 end = (ut64)h->fixupsize + h->fpagetab + bin->headerOff;
	while (offset < end) {
		RBinImport *imp = R_NEW0 (RBinImport);
		if (!imp) {
			break;
		}
		imp->name = __read_nonnull_str_at (bin->buf, &offset);
		if (!imp->name) {
			r_bin_import_free (imp);
			break;
		}
		imp->type = R_BIN_TYPE_FUNC_STR;
		r_list_append (l, imp);
	}
	return l;

}