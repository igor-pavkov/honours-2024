GF_Box *stsc_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_SampleToChunkBox, GF_ISOM_BOX_TYPE_STSC);
	return (GF_Box *)tmp;
}