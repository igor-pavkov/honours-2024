GF_Err gen_sample_entry_box_size(GF_Box *s)
{
	GF_SampleEntryBox *ptr = (GF_SampleEntryBox *)s;
	ptr->size += 8;
	return GF_OK;
}