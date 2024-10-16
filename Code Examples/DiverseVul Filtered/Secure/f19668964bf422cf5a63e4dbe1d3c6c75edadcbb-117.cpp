GF_Box *odrb_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_OMADRMRightsObjectBox, GF_ISOM_BOX_TYPE_ODRB);
	return (GF_Box *)tmp;
}