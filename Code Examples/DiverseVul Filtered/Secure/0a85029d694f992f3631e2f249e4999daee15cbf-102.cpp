GF_Err vpcc_box_read(GF_Box *s, GF_BitStream *bs)
{
	u64 pos;
	GF_VPConfigurationBox *ptr = (GF_VPConfigurationBox *)s;

	if (ptr->config) gf_odf_vp_cfg_del(ptr->config);
	ptr->config = NULL;

	pos = gf_bs_get_position(bs);
	ptr->config = gf_odf_vp_cfg_read_bs(bs, ptr->version == 0);
	pos = gf_bs_get_position(bs) - pos ;

	if (pos < ptr->size)
		GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[ISOBMFF] VPConfigurationBox: read only "LLU" bytes (expected "LLU").\n", pos, ptr->size));
	if (pos > ptr->size)
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[ISOBMFF] VPConfigurationBox overflow read "LLU" bytes, of box size "LLU".\n", pos, ptr->size));

	return ptr->config ? GF_OK : GF_ISOM_INVALID_FILE;
}