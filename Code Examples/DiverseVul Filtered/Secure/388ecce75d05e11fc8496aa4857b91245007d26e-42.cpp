GF_Box *stss_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_SyncSampleBox, GF_ISOM_BOX_TYPE_STSS);
	return (GF_Box*)tmp;
}