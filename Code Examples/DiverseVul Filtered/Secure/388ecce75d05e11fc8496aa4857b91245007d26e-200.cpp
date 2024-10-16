GF_Box *payt_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_PAYTBox, GF_ISOM_BOX_TYPE_PAYT);
	return (GF_Box *)tmp;
}