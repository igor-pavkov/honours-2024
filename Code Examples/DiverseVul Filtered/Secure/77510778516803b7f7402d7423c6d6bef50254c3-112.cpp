GF_Box *vwid_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_ViewIdentifierBox, GF_ISOM_BOX_TYPE_VWID);
	return (GF_Box *)tmp;
}