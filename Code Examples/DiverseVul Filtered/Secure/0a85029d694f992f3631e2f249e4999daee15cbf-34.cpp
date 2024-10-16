GF_Err SmDm_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_SMPTE2086MasteringDisplayMetadataBox *p = (GF_SMPTE2086MasteringDisplayMetadataBox *)s;
	ISOM_DECREASE_SIZE(p, 24)
	p->primaryRChromaticity_x = gf_bs_read_u16(bs);
	p->primaryRChromaticity_y = gf_bs_read_u16(bs);
	p->primaryGChromaticity_x = gf_bs_read_u16(bs);
	p->primaryGChromaticity_y = gf_bs_read_u16(bs);
	p->primaryBChromaticity_x = gf_bs_read_u16(bs);
	p->primaryBChromaticity_y = gf_bs_read_u16(bs);
	p->whitePointChromaticity_x = gf_bs_read_u16(bs);
	p->whitePointChromaticity_y = gf_bs_read_u16(bs);
	p->luminanceMax = gf_bs_read_u32(bs);
	p->luminanceMin = gf_bs_read_u32(bs);

	return GF_OK;
}