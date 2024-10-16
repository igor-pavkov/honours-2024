GF_Box *schm_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_SchemeTypeBox, GF_ISOM_BOX_TYPE_SCHM);
	return (GF_Box *)tmp;
}