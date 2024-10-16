GF_Box *gmin_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_GenericMediaHeaderInfoBox, GF_QT_BOX_TYPE_GMIN);
	return (GF_Box *)tmp;
}