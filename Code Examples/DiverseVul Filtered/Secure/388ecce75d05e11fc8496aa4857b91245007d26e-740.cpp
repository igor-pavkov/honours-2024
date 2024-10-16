void chpl_box_del(GF_Box *s)
{
	GF_ChapterListBox *ptr = (GF_ChapterListBox *) s;
	if (ptr == NULL) return;
	while (gf_list_count(ptr->list)) {
		GF_ChapterEntry *ce = (GF_ChapterEntry *)gf_list_get(ptr->list, 0);
		if (ce->name) gf_free(ce->name);
		gf_free(ce);
		gf_list_rem(ptr->list, 0);
	}
	gf_list_del(ptr->list);
	gf_free(ptr);
}