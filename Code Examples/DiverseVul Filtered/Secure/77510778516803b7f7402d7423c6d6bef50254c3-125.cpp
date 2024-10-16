GF_Err proj_type_box_size(GF_Box *s)
{
	GF_ProjectionTypeBox *ptr = (GF_ProjectionTypeBox *)s;
	if (ptr->type==GF_ISOM_BOX_TYPE_CBMP)
		s->size += 8;
	else if (ptr->type==GF_ISOM_BOX_TYPE_EQUI)
		s->size += 16;
	else
		s->size += 8;

	return GF_OK;
}