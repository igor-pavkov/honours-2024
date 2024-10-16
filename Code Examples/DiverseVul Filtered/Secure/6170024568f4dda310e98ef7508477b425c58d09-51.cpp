GF_Box *alis_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_DataEntryAliasBox, GF_QT_BOX_TYPE_ALIS);
	return (GF_Box *)tmp;
}