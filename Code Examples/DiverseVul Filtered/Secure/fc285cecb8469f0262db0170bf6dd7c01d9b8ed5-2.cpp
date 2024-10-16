static RBinXtrData *get_the_meta(RBin *bin, RBuffer *buf) {
	RBinXtrMetadata *meta = R_NEW0 (RBinXtrMetadata);
	meta->machine = "mono";
	meta->type = "assembly";
	meta->libname = NULL;
	meta->xtr_type = "xalz";

	ut32 osz = r_buf_read_le32_at (buf, 8);
	int datalen = r_buf_size (buf) - 0xc;
	ut8 *data = malloc (datalen);
	if (!data) {
		free (meta);
		return NULL;
	}
	r_buf_read_at (buf, 0xc, data, datalen);
	int consumed = 0;
	int outsize = 0;
	ut8 *obuf = r_inflate_lz4 (data, datalen, &consumed, &outsize);
	if (obuf && outsize == osz) {
		buf = r_buf_new_with_pointers (obuf, outsize, true);
		RBinXtrData *res = r_bin_xtrdata_new (buf, 0, r_buf_size (buf), 0, meta);
		free (data);
		return res;
	}
	R_LOG_ERROR ("LZ4 decompression failed");
	free (data);
	free (meta);
	free (obuf);
	return NULL;
}