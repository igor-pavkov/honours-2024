GF_Box *srpp_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_SRTPProcessBox, GF_ISOM_BOX_TYPE_SRPP);
	return (GF_Box *)tmp;
}