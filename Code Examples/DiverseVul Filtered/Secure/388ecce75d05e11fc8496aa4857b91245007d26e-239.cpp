GF_Box *tims_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_TSHintEntryBox, GF_ISOM_BOX_TYPE_TIMS);
	return (GF_Box *)tmp;
}