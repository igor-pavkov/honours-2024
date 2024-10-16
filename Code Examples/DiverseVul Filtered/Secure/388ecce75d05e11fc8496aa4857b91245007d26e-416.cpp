GF_Box *stsd_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_SampleDescriptionBox, GF_ISOM_BOX_TYPE_STSD);
	tmp->child_boxes = gf_list_new();
	return (GF_Box *)tmp;
}