GF_Box *paen_box_new()
{
	ISOM_DECL_BOX_ALLOC(FDPartitionEntryBox, GF_ISOM_BOX_TYPE_PAEN);
	return (GF_Box *)tmp;
}