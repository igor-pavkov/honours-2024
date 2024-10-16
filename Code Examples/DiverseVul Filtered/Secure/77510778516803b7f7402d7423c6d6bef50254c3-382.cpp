GF_Err gitn_box_size(GF_Box *s)
{
	u32 i;
	GroupIdToNameBox *ptr = (GroupIdToNameBox *)s;
	ptr->size += 2;

	for (i=0; i<ptr->nb_entries; i++) {
		ptr->size += 5;
		if (ptr->entries[i].name) ptr->size += strlen(ptr->entries[i].name);
	}
	return GF_OK;
}