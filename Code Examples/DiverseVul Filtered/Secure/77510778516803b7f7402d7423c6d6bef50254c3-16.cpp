void mhac_box_del(GF_Box *s)
{
	GF_MHAConfigBox *ptr = (GF_MHAConfigBox *) s;
	if (ptr->mha_config) gf_free(ptr->mha_config);
	gf_free(s);
}