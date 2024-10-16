GF_Box *ohdr_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_OMADRMCommonHeaderBox, GF_ISOM_BOX_TYPE_OHDR);
	tmp->child_boxes = gf_list_new();
	return (GF_Box *)tmp;
}