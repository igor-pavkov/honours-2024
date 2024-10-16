GF_Box *adaf_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_AdobeDRMAUFormatBox, GF_ISOM_BOX_TYPE_ADAF);
	return (GF_Box *)tmp;
}