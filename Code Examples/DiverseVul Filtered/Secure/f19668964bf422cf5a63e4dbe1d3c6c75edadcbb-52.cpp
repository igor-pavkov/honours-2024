void odrb_box_del(GF_Box *s)
{
	GF_OMADRMRightsObjectBox *ptr = (GF_OMADRMRightsObjectBox*)s;
	if (ptr->oma_ro) gf_free(ptr->oma_ro);
	gf_free(ptr);
}