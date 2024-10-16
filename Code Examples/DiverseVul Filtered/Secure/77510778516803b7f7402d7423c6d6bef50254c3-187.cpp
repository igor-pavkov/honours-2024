GF_Box *fpar_box_new()
{
	ISOM_DECL_BOX_ALLOC(FilePartitionBox, GF_ISOM_BOX_TYPE_FPAR);
	return (GF_Box *)tmp;
}