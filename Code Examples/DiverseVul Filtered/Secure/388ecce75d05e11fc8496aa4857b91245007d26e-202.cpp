GF_Box *urn_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_DataEntryURNBox, GF_ISOM_BOX_TYPE_URN);
	return (GF_Box *)tmp;
}