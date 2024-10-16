GF_Box *ilst_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_ItemListBox, GF_ISOM_BOX_TYPE_ILST);
	tmp->child_boxes = gf_list_new();
	return (GF_Box *)tmp;
}