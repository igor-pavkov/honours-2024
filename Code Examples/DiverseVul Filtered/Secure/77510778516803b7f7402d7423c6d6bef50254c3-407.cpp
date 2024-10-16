GF_Err fpar_box_size(GF_Box *s)
{
	FilePartitionBox *ptr = (FilePartitionBox *)s;

	ptr->size += 13 + (ptr->version ? 8 : 4);
	if (ptr->scheme_specific_info)
		ptr->size += strlen(ptr->scheme_specific_info);

	ptr->size+= ptr->nb_entries * 6;
	return GF_OK;
}