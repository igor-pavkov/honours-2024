GF_Err frma_box_size(GF_Box *s)
{
	GF_OriginalFormatBox *ptr = (GF_OriginalFormatBox *)s;
	ptr->size += 4;
	return GF_OK;
}