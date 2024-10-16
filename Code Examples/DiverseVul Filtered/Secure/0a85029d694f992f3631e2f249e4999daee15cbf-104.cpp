GF_Err hvcc_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_HEVCConfigurationBox *ptr = (GF_HEVCConfigurationBox *) s;
	if (!s) return GF_BAD_PARAM;
	if (!ptr->config) return GF_OK;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;

	return gf_odf_hevc_cfg_write_bs(ptr->config, bs);
}