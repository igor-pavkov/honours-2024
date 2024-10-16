GF_Box *chan_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_ChannelLayoutInfoBox, GF_QT_BOX_TYPE_CHAN);
	return (GF_Box *)tmp;
}