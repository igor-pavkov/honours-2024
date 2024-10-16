GF_Box *trex_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_TrackExtendsBox, GF_ISOM_BOX_TYPE_TREX);
	return (GF_Box *)tmp;
}