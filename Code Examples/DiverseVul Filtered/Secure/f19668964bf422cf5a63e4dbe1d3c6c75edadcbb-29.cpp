GF_Box *iSLT_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_ISMACrypSaltBox, GF_ISOM_BOX_TYPE_ISLT);
	return (GF_Box *)tmp;
}