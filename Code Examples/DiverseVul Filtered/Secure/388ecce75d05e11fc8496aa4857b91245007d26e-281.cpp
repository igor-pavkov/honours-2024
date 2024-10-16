GF_Err srpp_box_size(GF_Box *s)
{
	u32 pos = 0;
	GF_SRTPProcessBox *ptr = (GF_SRTPProcessBox *)s;
	s->size += 16;
	gf_isom_check_position(s, (GF_Box*)ptr->info, &pos);
	gf_isom_check_position(s, (GF_Box*)ptr->scheme_type, &pos);
	return GF_OK;
}