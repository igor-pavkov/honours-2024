GF_Box *chnl_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_ChannelLayoutBox, GF_ISOM_BOX_TYPE_CHNL);
	return (GF_Box *)tmp;
}