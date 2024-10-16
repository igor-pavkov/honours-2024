GF_Err dvcC_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_DOVIConfigurationBox *ptr = (GF_DOVIConfigurationBox *) s;
	if (!s) return GF_BAD_PARAM;

	e = gf_isom_box_write_header(s, bs);
	if (e) return e;

	//GF_DOVIDecoderConfigurationRecord
	gf_bs_write_u8(bs,  ptr->DOVIConfig.dv_version_major);
	gf_bs_write_u8(bs,  ptr->DOVIConfig.dv_version_minor);
	gf_bs_write_int(bs, ptr->DOVIConfig.dv_profile, 7);
	gf_bs_write_int(bs, ptr->DOVIConfig.dv_level, 6);
	gf_bs_write_int(bs, ptr->DOVIConfig.rpu_present_flag, 1);
	gf_bs_write_int(bs, ptr->DOVIConfig.el_present_flag, 1);
	gf_bs_write_int(bs, ptr->DOVIConfig.bl_present_flag, 1);
	gf_bs_write_int(bs, ptr->DOVIConfig.dv_bl_signal_compatibility_id, 4);
	gf_bs_write_int(bs, 0, 28);
	gf_bs_write_u32(bs, 0);
	gf_bs_write_u32(bs, 0);
	gf_bs_write_u32(bs, 0);
	gf_bs_write_u32(bs, 0);

	return GF_OK;
}