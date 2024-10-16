GF_Err odrb_box_size(GF_Box *s)
{
	GF_OMADRMRightsObjectBox *ptr = (GF_OMADRMRightsObjectBox *)s;
	s->size += ptr->oma_ro_size;
	return GF_OK;
}