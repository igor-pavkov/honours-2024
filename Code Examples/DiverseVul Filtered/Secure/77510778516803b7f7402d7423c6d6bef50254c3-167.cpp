GF_Box *pcrb_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_PcrInfoBox, GF_ISOM_BOX_TYPE_PCRB);
	return (GF_Box *)tmp;
}