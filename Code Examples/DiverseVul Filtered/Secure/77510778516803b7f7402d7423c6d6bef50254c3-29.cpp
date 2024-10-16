GF_Box *trak_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_TrackBox, GF_ISOM_BOX_TYPE_TRAK);
	return (GF_Box *)tmp;
}