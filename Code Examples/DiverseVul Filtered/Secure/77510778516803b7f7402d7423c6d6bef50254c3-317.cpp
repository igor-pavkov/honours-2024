GF_Box *dfla_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_FLACConfigBox, GF_ISOM_BOX_TYPE_DFLA);
	return (GF_Box *)tmp;
}