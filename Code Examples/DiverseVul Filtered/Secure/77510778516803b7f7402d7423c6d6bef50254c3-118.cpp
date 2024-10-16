GF_Box *sgpd_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_SampleGroupDescriptionBox, GF_ISOM_BOX_TYPE_SGPD);
	/*version 0 is deprecated, use v1 by default*/
	tmp->version = 1;
	tmp->group_descriptions = gf_list_new();
	return (GF_Box *)tmp;
}