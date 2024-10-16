GF_Err ftyp_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	GF_FileTypeBox *ptr = (GF_FileTypeBox *) s;

	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_u32(bs, ptr->majorBrand);
	gf_bs_write_u32(bs, ptr->minorVersion);
	for (i=0; i<ptr->altCount; i++) {
		gf_bs_write_u32(bs, ptr->altBrand[i]);
	}
	return GF_OK;
}