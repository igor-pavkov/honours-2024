GF_Err grpi_box_size(GF_Box *s)
{
	GF_OMADRMGroupIDBox *ptr = (GF_OMADRMGroupIDBox *)s;
	ptr->size += 2+2+1 + ptr->GKLength;
	if (ptr->GroupID) ptr->size += strlen(ptr->GroupID);
	return GF_OK;
}