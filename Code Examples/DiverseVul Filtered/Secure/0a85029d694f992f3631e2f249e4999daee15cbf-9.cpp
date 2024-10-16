GF_Err SmDm_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_SMPTE2086MasteringDisplayMetadataBox *p = (GF_SMPTE2086MasteringDisplayMetadataBox*)s;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	gf_bs_write_u16(bs, p->primaryRChromaticity_x);
	gf_bs_write_u16(bs, p->primaryRChromaticity_y);
	gf_bs_write_u16(bs, p->primaryGChromaticity_x);
	gf_bs_write_u16(bs, p->primaryGChromaticity_y);
	gf_bs_write_u16(bs, p->primaryBChromaticity_x);
	gf_bs_write_u16(bs, p->primaryBChromaticity_y);
	gf_bs_write_u16(bs, p->whitePointChromaticity_x);
	gf_bs_write_u16(bs, p->whitePointChromaticity_y);
	gf_bs_write_u32(bs, p->luminanceMax);
	gf_bs_write_u32(bs, p->luminanceMin);

	return GF_OK;
}