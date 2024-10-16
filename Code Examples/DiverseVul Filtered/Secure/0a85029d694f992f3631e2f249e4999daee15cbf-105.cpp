GF_Err vvcc_box_read(GF_Box *s, GF_BitStream *bs)
{
	u64 consumed;
	GF_VVCConfigurationBox *ptr = (GF_VVCConfigurationBox *)s;

	if (ptr->config) gf_odf_vvc_cfg_del(ptr->config);

	consumed = gf_bs_get_position(bs);
	ptr->config = gf_odf_vvc_cfg_read_bs(bs);
	consumed = gf_bs_get_position(bs) - consumed ;
	ISOM_DECREASE_SIZE(ptr, (u32)consumed)

	return ptr->config ? GF_OK : GF_ISOM_INVALID_FILE;
}