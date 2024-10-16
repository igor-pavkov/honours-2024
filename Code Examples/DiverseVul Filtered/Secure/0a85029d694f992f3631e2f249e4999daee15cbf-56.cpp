GF_Err btrt_box_size(GF_Box *s)
{
	GF_BitRateBox *ptr = (GF_BitRateBox *)s;
	ptr->size += 12;
	return GF_OK;
}