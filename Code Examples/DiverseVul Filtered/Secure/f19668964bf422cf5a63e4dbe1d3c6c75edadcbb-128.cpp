void adkm_box_del(GF_Box *s)
{
	GF_AdobeDRMKeyManagementSystemBox *ptr = (GF_AdobeDRMKeyManagementSystemBox *)s;
	if (!ptr) return;
	gf_free(s);
}