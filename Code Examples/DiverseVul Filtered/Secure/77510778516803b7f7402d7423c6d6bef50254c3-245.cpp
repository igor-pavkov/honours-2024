GF_Err lsr1_box_size(GF_Box *s)
{
	u32 pos=0;
	GF_LASeRSampleEntryBox *ptr = (GF_LASeRSampleEntryBox *)s;
	s->size += 8;
	gf_isom_check_position(s, (GF_Box *)ptr->lsr_config, &pos);
	return GF_OK;
}