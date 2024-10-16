RList *r_bin_le_get_symbols(r_bin_le_obj_t *bin) {
	RList *l = r_list_newf ((RListFree)r_bin_symbol_free);
	RList *entries = __get_entries (bin);
	LE_image_header *h = bin->header;
	ut64 offset = (ut64)h->restab + bin->headerOff;
	ut32 end = h->enttab + bin->headerOff;
	__get_symbols_at (bin, l, entries, offset, end);
	offset = h->nrestab;
	end = h->nrestab + h->cbnrestab;
	__get_symbols_at (bin, l, entries, offset, end);
	r_list_free (entries);
	return l;
}