GF_Err CoLL_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_VPContentLightLevelBox *p = (GF_VPContentLightLevelBox *)s;
	ISOM_DECREASE_SIZE(p, 4)
	p->maxCLL = gf_bs_read_u16(bs);
	p->maxFALL = gf_bs_read_u16(bs);

	return GF_OK;
}