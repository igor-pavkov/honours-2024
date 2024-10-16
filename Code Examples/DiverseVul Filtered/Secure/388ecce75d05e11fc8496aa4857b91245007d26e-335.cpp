GF_Err edts_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_EditBox *ptr = (GF_EditBox *)s;

	//here we have a trick: if editList is empty, skip the box
	if (ptr->editList && gf_list_count(ptr->editList->entryList)) {
		return gf_isom_box_write_header(s, bs);
	} else {
		s->size = 0;
	}
	return GF_OK;
}