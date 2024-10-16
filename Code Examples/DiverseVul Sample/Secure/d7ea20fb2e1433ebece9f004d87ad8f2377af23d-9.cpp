r_bin_le_obj_t *r_bin_le_new_buf(RBuffer *buf) {
	r_bin_le_obj_t *bin = R_NEW0 (r_bin_le_obj_t);
	if (!bin) {
		return NULL;
	}

	__init_header (bin, buf);
	LE_image_header *h = bin->header;
	if (!memcmp ("LE", h->magic, 2)) {
		bin->is_le = true;
	}
	bin->type = __get_module_type (bin);
	bin->cpu = __get_cpu_type (bin);
	bin->os = __get_os_type (bin);
	bin->arch = __get_arch (bin);
	bin->objtbl = calloc (h->objcnt, sizeof (LE_object_entry));
	if (!bin->objtbl) {
		r_bin_le_free (bin);
		return NULL;
	}
	ut64 offset = (ut64)bin->headerOff + h->restab;
	bin->filename = __read_nonnull_str_at (buf, &offset);
	r_buf_read_at (buf, (ut64)h->objtab + bin->headerOff, (ut8 *)bin->objtbl, h->objcnt * sizeof (LE_object_entry));

	bin->buf = buf;
	return bin;
}