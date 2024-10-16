GF_Err adaf_box_size(GF_Box *s)
{
	GF_AdobeDRMAUFormatBox *ptr = (GF_AdobeDRMAUFormatBox*)s;
	ptr->size += 3;
	return GF_OK;
}