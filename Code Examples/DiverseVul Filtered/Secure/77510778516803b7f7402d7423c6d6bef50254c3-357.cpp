GF_Box *sidx_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_SegmentIndexBox, GF_ISOM_BOX_TYPE_SIDX);
	return (GF_Box *)tmp;
}