GF_Box *ahdr_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_AdobeDRMHeaderBox, GF_ISOM_BOX_TYPE_AHDR);
	tmp->version = 2;
	tmp->flags = 0;
	return (GF_Box *)tmp;
}