GF_Err tmcd_box_size(GF_Box *s)
{
	GF_SampleEntryBox *ptr = (GF_SampleEntryBox *)s;
	ptr->size += 8 + 18;
	return GF_OK;
}