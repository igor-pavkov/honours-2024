void pcrb_box_del(GF_Box *s)
{
	GF_PcrInfoBox *ptr = (GF_PcrInfoBox *) s;
	if (ptr == NULL) return;
	if (ptr->pcr_values) gf_free(ptr->pcr_values);
	gf_free(ptr);
}