GF_Box *fiel_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_FieldInfoBox, GF_QT_BOX_TYPE_FIEL);
	return (GF_Box *)tmp;
}