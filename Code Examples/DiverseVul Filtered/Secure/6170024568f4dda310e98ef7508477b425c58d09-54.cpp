GF_Box *databox_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_DataBox, GF_ISOM_BOX_TYPE_DATA);

	return (GF_Box *)tmp;
}