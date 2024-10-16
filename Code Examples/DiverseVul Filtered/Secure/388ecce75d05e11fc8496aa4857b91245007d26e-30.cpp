GF_Box *rssr_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_ReceivedSsrcBox, GF_ISOM_BOX_TYPE_RSSR);
	return (GF_Box *)tmp;
}