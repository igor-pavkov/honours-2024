GF_Err dac3_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_AC3ConfigBox *ptr = (GF_AC3ConfigBox *)s;

	if (ptr->cfg.is_ec3) s->type = GF_ISOM_BOX_TYPE_DEC3;
	e = gf_isom_box_write_header(s, bs);
	if (ptr->cfg.is_ec3) s->type = GF_ISOM_BOX_TYPE_DAC3;
	if (e) return e;
	
	return gf_odf_ac3_cfg_write_bs(&ptr->cfg, bs);
}