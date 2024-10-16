GF_Box *emsg_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_EventMessageBox, GF_ISOM_BOX_TYPE_EMSG);
	return (GF_Box *)tmp;
}