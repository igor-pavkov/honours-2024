GF_Err padb_box_size(GF_Box *s)
{
	GF_PaddingBitsBox *ptr = (GF_PaddingBitsBox *)s;
	ptr->size += 4;
	if (ptr->SampleCount) ptr->size += (ptr->SampleCount + 1) / 2;

	return GF_OK;
}