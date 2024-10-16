GF_Err mfhd_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_MovieFragmentHeaderBox *ptr = (GF_MovieFragmentHeaderBox *) s;
	if (!s) return GF_BAD_PARAM;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_u32(bs, ptr->sequence_number);
	return GF_OK;
}