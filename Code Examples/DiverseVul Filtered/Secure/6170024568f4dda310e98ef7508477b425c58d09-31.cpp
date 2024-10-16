GF_Box *clef_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_ApertureBox, GF_QT_BOX_TYPE_CLEF);
	return (GF_Box *)tmp;
}