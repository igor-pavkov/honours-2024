GF_Err cslg_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_CompositionToDecodeBox *ptr = (GF_CompositionToDecodeBox *)s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_int(bs, ptr->compositionToDTSShift, 32);
	gf_bs_write_int(bs, ptr->leastDecodeToDisplayDelta, 32);
	gf_bs_write_int(bs, ptr->greatestDecodeToDisplayDelta, 32);
	gf_bs_write_int(bs, ptr->compositionStartTime, 32);
	gf_bs_write_int(bs, ptr->compositionEndTime, 32);
	return GF_OK;
}