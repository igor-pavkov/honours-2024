GF_Box *ilst_item_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_ListItemBox, GF_ISOM_BOX_TYPE_CPIL); //type will be overwrite
	tmp->data = (GF_DataBox *)gf_isom_box_new_parent(&tmp->child_boxes, GF_ISOM_BOX_TYPE_DATA);
	if (tmp->data == NULL) {
		if (tmp->child_boxes) gf_list_del(tmp->child_boxes);
		gf_free(tmp);
		return NULL;
	}
	return (GF_Box *)tmp;
}