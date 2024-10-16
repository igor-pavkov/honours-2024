GF_Box *svhd_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_SphericalVideoInfoBox, GF_ISOM_BOX_TYPE_SVHD);
	return (GF_Box *)tmp;
}