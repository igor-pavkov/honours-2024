GF_Box *ssix_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_SubsegmentIndexBox, GF_ISOM_BOX_TYPE_SSIX);
	return (GF_Box *)tmp;
}