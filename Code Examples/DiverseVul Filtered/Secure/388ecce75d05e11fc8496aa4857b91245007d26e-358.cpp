GF_Err cprt_box_size(GF_Box *s)
{
	GF_CopyrightBox *ptr = (GF_CopyrightBox *)s;

	ptr->size += 2;
	if (ptr->notice)
		ptr->size += strlen(ptr->notice) + 1;
	return GF_OK;
}