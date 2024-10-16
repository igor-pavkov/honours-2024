GF_Err edts_box_size(GF_Box *s)
{
	GF_EditBox *ptr = (GF_EditBox *)s;

	//here we have a trick: if editList is empty, skip the box
	if (!ptr->editList || ! gf_list_count(ptr->editList->entryList)) {
		ptr->size = 0;
	}
	return GF_OK;
}