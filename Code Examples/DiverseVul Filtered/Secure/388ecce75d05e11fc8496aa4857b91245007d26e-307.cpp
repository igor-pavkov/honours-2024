GF_Err stsd_box_size(GF_Box *s)
{
	GF_SampleDescriptionBox *ptr = (GF_SampleDescriptionBox *)s;
	ptr->size += 4;
	return GF_OK;
}