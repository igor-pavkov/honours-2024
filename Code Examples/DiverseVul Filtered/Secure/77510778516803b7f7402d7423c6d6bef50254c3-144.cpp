GF_Box *reftype_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_TrackReferenceTypeBox, GF_ISOM_BOX_TYPE_REFT);
	return (GF_Box *)tmp;
}