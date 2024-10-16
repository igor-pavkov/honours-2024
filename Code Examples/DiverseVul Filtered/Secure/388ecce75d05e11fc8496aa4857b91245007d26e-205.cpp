GF_Err mvex_box_size(GF_Box *s)
{
	u32 pos=0;
	GF_MovieExtendsBox *ptr = (GF_MovieExtendsBox *) s;
	gf_isom_check_position(s, (GF_Box *)ptr->mehd, &pos);
	gf_isom_check_position_list(s, ptr->TrackExList, &pos);
	gf_isom_check_position_list(s, ptr->TrackExPropList, &pos);
	return GF_OK;
}