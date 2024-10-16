GF_Box *tref_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_TrackReferenceBox, GF_ISOM_BOX_TYPE_TREF);
	return (GF_Box *)tmp;
}