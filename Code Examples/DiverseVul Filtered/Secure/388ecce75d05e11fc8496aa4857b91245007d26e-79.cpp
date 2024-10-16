GF_Err mdat_box_size(GF_Box *s)
{
	GF_MediaDataBox *ptr = (GF_MediaDataBox *)s;
	ptr->size += ptr->dataSize;
	return GF_OK;
}