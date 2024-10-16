GF_Box *strk_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_SubTrackBox, GF_ISOM_BOX_TYPE_STRK);
	return (GF_Box *)tmp;
}