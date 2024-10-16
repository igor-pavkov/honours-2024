GF_Err stts_box_size(GF_Box *s)
{
	GF_TimeToSampleBox *ptr = (GF_TimeToSampleBox *)s;
	ptr->size += 4 + (8 * ptr->nb_entries);
	return GF_OK;
}