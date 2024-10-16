GF_Box *csgp_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_CompactSampleGroupBox, GF_ISOM_BOX_TYPE_CSGP);
	return (GF_Box *)tmp;
}