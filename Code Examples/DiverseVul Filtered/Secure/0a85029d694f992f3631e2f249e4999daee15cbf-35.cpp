GF_Err av1c_box_read(GF_Box *s, GF_BitStream *bs)
{
	u64 pos, read;
	GF_AV1ConfigurationBox *ptr = (GF_AV1ConfigurationBox*)s;

	if (ptr->config) gf_odf_av1_cfg_del(ptr->config);

	pos = gf_bs_get_position(bs);

	ptr->config = gf_odf_av1_cfg_read_bs_size(bs, (u32) ptr->size);

	read = gf_bs_get_position(bs) - pos;

	if (read < ptr->size)
		GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[ISOBMFF] AV1ConfigurationBox: read only "LLU" bytes (expected "LLU").\n", read, ptr->size));
	if (read > ptr->size)
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[ISOBMFF] AV1ConfigurationBox overflow read "LLU" bytes, of box size "LLU".\n", read, ptr->size));

	return GF_OK;
}