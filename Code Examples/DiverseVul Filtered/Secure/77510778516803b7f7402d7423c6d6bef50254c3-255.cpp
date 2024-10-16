GF_Err mhac_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_MHAConfigBox *ptr = (GF_MHAConfigBox *) s;

	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_u8(bs, ptr->configuration_version);
	gf_bs_write_u8(bs, ptr->mha_pl_indication);
	gf_bs_write_u8(bs, ptr->reference_channel_layout);
	gf_bs_write_u16(bs, ptr->mha_config ? ptr->mha_config_size : 0);
	if (ptr->mha_config && ptr->mha_config_size)
		gf_bs_write_data(bs, ptr->mha_config, ptr->mha_config_size);

	return GF_OK;
}