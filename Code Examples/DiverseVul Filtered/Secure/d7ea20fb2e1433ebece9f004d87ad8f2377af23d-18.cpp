static bool __init_header(r_bin_le_obj_t *bin, RBuffer *buf) {
	ut8 magic[2];
	r_buf_read_at (buf, 0, magic, sizeof (magic));
	if (!memcmp (&magic, "MZ", 2)) {
		bin->headerOff = r_buf_read_le16_at (buf, 0x3c);
	} else {
		bin->headerOff = 0;
	}
	bin->header = R_NEW0 (LE_image_header);
	if (bin->header) {
		r_buf_read_at (buf, bin->headerOff, (ut8 *)bin->header, sizeof (LE_image_header));
	} else {
		R_LOG_ERROR ("Failed to allocate memory");
		return false;
	}
	return true;
}