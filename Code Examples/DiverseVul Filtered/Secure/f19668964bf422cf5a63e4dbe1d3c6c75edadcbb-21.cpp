GF_Err iSFM_box_size(GF_Box *s)
{
	GF_ISMASampleFormatBox *ptr = (GF_ISMASampleFormatBox *)s;
	ptr->size += 3;
	return GF_OK;
}