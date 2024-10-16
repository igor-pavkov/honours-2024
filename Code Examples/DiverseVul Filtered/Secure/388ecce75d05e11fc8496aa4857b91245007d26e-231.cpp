GF_Box *hinf_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_HintInfoBox, GF_ISOM_BOX_TYPE_HINF);

	tmp->child_boxes = gf_list_new();
	return (GF_Box *)tmp;
}