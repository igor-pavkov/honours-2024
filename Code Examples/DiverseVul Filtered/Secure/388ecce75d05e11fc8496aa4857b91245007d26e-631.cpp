GF_Err cslg_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_CompositionToDecodeBox *ptr = (GF_CompositionToDecodeBox *)s;

	ISOM_DECREASE_SIZE(ptr, 20);
	ptr->compositionToDTSShift = gf_bs_read_int(bs, 32);
	ptr->leastDecodeToDisplayDelta = gf_bs_read_int(bs, 32);
	ptr->greatestDecodeToDisplayDelta = gf_bs_read_int(bs, 32);
	ptr->compositionStartTime = gf_bs_read_int(bs, 32);
	ptr->compositionEndTime = gf_bs_read_int(bs, 32);
	return GF_OK;
}