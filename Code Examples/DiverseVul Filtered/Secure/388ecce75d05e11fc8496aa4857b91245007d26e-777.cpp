GF_Box *chpl_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_ChapterListBox, GF_ISOM_BOX_TYPE_CHPL);
	tmp->list = gf_list_new();
	tmp->version = 1;
	return (GF_Box *)tmp;
}