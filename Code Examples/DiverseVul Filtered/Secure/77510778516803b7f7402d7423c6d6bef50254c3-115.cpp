GF_Box *tsro_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_TimeOffHintEntryBox, GF_ISOM_BOX_TYPE_TSRO);
	return (GF_Box *)tmp;
}