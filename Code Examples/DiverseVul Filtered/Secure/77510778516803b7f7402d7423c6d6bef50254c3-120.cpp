GF_Box *gitn_box_new()
{
	ISOM_DECL_BOX_ALLOC(GroupIdToNameBox, GF_ISOM_BOX_TYPE_GITN);
	return (GF_Box *)tmp;
}