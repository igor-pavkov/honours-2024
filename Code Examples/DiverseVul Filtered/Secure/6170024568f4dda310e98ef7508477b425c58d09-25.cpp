GF_Err tcmi_box_size(GF_Box *s)
{
	GF_TimeCodeMediaInformationBox *ptr = (GF_TimeCodeMediaInformationBox *)s;
	ptr->size += 21;
	if (ptr->font)
    	ptr->size += strlen(ptr->font);
	return GF_OK;
}