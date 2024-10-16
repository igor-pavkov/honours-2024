void ohdr_box_del(GF_Box *s)
{
	GF_OMADRMCommonHeaderBox *ptr = (GF_OMADRMCommonHeaderBox*)s;
	if (ptr == NULL) return;
	if (ptr->ContentID) gf_free(ptr->ContentID);
	if (ptr->RightsIssuerURL) gf_free(ptr->RightsIssuerURL);
	if (ptr->TextualHeaders) gf_free(ptr->TextualHeaders);
	gf_free(ptr);
}