void pdin_box_del(GF_Box *s)
{
	GF_ProgressiveDownloadBox *ptr = (GF_ProgressiveDownloadBox*)s;
	if (ptr == NULL) return;
	if (ptr->rates) gf_free(ptr->rates);
	if (ptr->times) gf_free(ptr->times);
	gf_free(ptr);
}