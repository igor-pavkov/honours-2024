GF_Err trgr_box_size(GF_Box *s)
{
	u32 pos=0;
	GF_TrackGroupBox *ptr = (GF_TrackGroupBox *) s;
	gf_isom_check_position_list(s, ptr->groups, &pos);
	return GF_OK;
}