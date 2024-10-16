GF_Box *dref_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_DataReferenceBox, GF_ISOM_BOX_TYPE_DREF);
	return (GF_Box *)tmp;
}