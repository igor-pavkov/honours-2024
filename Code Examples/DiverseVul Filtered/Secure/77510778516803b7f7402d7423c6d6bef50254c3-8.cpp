void ainf_box_del(GF_Box *s)
{
	GF_AssetInformationBox *ptr = (GF_AssetInformationBox *) s;
	if (ptr->APID) gf_free(ptr->APID);
	gf_free(s);
}