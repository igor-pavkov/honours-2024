GF_VVCConfig *VVC_DuplicateConfig(GF_VVCConfig *cfg)
{
	u8 *data;
	u32 data_size;
	GF_VVCConfig *new_cfg;
	GF_BitStream *bs;

	if (!cfg) return NULL;
	bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
	gf_odf_vvc_cfg_write_bs(cfg, bs);

	gf_bs_get_content(bs, &data, &data_size);
	gf_bs_del(bs);
	bs = gf_bs_new(data, data_size, GF_BITSTREAM_READ);

	new_cfg = gf_odf_vvc_cfg_read_bs(bs);
	gf_bs_del(bs);
	gf_free(data);
	return new_cfg;
}