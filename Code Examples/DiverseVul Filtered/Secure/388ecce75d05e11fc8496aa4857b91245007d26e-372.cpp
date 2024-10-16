GF_Box *snro_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_SeqOffHintEntryBox, GF_ISOM_BOX_TYPE_SNRO);
	return (GF_Box *)tmp;
}