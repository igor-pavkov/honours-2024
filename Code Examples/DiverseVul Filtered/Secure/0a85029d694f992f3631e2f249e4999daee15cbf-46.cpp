GF_Err av1c_box_write(GF_Box *s, GF_BitStream *bs) {
	GF_Err e;
	GF_AV1ConfigurationBox *ptr = (GF_AV1ConfigurationBox*)s;
	if (!s) return GF_BAD_PARAM;
	if (!ptr->config) return GF_BAD_PARAM;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;

	return gf_odf_av1_cfg_write_bs(ptr->config, bs);
}