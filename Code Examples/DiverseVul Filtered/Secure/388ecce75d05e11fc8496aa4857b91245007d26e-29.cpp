GF_Err padb_box_write(GF_Box *s, GF_BitStream *bs)
{
	u32 i;
	GF_Err e;
	GF_PaddingBitsBox *ptr = (GF_PaddingBitsBox *) s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_int(bs, ptr->SampleCount, 32);

	for (i=0 ; i<ptr->SampleCount; i += 2) {
		gf_bs_write_int(bs, 0, 1);
		if (i+1 < ptr->SampleCount) {
			gf_bs_write_int(bs, ptr->padbits[i+1], 3);
		} else {
			gf_bs_write_int(bs, 0, 3);
		}
		gf_bs_write_int(bs, 0, 1);
		gf_bs_write_int(bs, ptr->padbits[i], 3);
	}
	return GF_OK;
}