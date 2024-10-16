void flxs_box_del(GF_Box *s)
{
	GF_AdobeFlashAccessParamsBox *ptr = (GF_AdobeFlashAccessParamsBox*)s;
	if (!ptr) return;
	if (ptr->metadata)
		gf_free(ptr->metadata);
	gf_free(ptr);
}