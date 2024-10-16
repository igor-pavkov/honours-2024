GF_Err fiin_box_size(GF_Box *s)
{
	u32 pos=0;
	FDItemInformationBox *ptr = (FDItemInformationBox *) s;
	s->size+= 2;
	gf_isom_check_position_list(s, ptr->partition_entries, &pos);
	return GF_OK;
}