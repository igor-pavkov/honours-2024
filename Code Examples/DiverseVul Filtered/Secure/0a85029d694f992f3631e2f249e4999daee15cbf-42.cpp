GF_Err CoLL_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_VPContentLightLevelBox *p = (GF_VPContentLightLevelBox*)s;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	gf_bs_write_u16(bs, p->maxCLL);
	gf_bs_write_u16(bs, p->maxFALL);

	return GF_OK;
}