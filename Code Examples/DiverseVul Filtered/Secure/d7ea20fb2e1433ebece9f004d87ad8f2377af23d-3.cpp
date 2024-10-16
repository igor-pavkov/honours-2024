RList *r_bin_le_get_libs(r_bin_le_obj_t *bin) {
	RList *l = r_list_newf ((RListFree)free);
	if (!l) {
		return NULL;
	}
	LE_image_header *h = bin->header;
	ut64 offset = (ut64)h->impmod + bin->headerOff;
	ut64 end = offset + h->impproc - h->impmod;
	while (offset < end) {
		char *name = __read_nonnull_str_at (bin->buf, &offset);
		if (!name) {
			break;
		}
		r_list_append (l, name);
	}
	return l;
}