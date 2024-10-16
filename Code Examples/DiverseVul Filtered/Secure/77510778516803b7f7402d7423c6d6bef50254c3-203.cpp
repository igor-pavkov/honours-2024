GF_Err rvcc_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_RVCConfigurationBox *ptr = (GF_RVCConfigurationBox*) s;

	e = gf_isom_box_write_header(s, bs);
	if (e) return e;

	gf_bs_write_u16(bs, ptr->predefined_rvc_config);
	if (!ptr->predefined_rvc_config) {
		gf_bs_write_u16(bs, ptr->rvc_meta_idx);
	}
	return GF_OK;
}