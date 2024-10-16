GF_Err dac3_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_AC3ConfigBox *ptr = (GF_AC3ConfigBox *)s;
	if (ptr == NULL) return GF_BAD_PARAM;
	return gf_odf_ac3_config_parse_bs(bs, ptr->cfg.is_ec3, &ptr->cfg);
}