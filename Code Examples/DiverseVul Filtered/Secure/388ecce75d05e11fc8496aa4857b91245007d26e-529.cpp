GF_Box *hdlr_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_HandlerBox, GF_ISOM_BOX_TYPE_HDLR);
	return (GF_Box *)tmp;
}