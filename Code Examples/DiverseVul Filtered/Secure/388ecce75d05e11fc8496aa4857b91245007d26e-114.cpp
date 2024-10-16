GF_Err chpl_box_size(GF_Box *s)
{
	u32 count, i;
	GF_ChapterListBox *ptr = (GF_ChapterListBox *)s;

	ptr->size += 5;

	count = gf_list_count(ptr->list);
	for (i=0; i<count; i++) {
		GF_ChapterEntry *ce = (GF_ChapterEntry *)gf_list_get(ptr->list, i);
		ptr->size += 9; /*64bit time stamp + 8bit str len*/
		if (ce->name) ptr->size += strlen(ce->name);
	}
	return GF_OK;
}