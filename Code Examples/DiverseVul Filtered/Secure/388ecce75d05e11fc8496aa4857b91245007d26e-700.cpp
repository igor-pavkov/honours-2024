GF_Box *tssy_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_TimeStampSynchronyBox, GF_ISOM_BOX_TYPE_TSSY);
	return (GF_Box *)tmp;
}