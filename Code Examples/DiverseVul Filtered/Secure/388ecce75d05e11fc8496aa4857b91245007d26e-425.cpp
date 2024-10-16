GF_Box *co64_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_ChunkLargeOffsetBox, GF_ISOM_BOX_TYPE_CO64);
	return (GF_Box *)tmp;
}