GF_Box *flxs_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_AdobeFlashAccessParamsBox, GF_ISOM_BOX_TYPE_FLXS);
	return (GF_Box *)tmp;
}