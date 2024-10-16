GF_Err moov_box_size(GF_Box *s)
{
	u32 pos=0;
	GF_MovieBox *ptr = (GF_MovieBox *)s;

	gf_isom_check_position(s, (GF_Box *) ptr->mvhd, &pos);
	gf_isom_check_position(s, (GF_Box *) ptr->iods, &pos);
	gf_isom_check_position(s, (GF_Box *) ptr->meta, &pos);
#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	if (ptr->mvex && !ptr->mvex_after_traks) {
		gf_isom_check_position(s, (GF_Box *) ptr->mvex, &pos);
	}
#endif
	gf_isom_check_position_list(s, ptr->trackList, &pos);

#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	if (ptr->mvex && ptr->mvex_after_traks) {
		gf_isom_check_position(s, (GF_Box *) ptr->mvex, &pos);
	}
#endif
	return GF_OK;
}