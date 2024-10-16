GF_Err mfra_box_size(GF_Box *s)
{
	u32 pos=0;
	GF_MovieFragmentRandomAccessBox *ptr = (GF_MovieFragmentRandomAccessBox *)s;
	gf_isom_check_position_list(s, ptr->tfra_list, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->mfro, &pos);
	return GF_OK;
}