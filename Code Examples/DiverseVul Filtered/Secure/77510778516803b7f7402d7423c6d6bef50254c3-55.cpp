void dvcC_box_del(GF_Box *s)
{
	GF_DOVIConfigurationBox *ptr = (GF_DOVIConfigurationBox*)s;
	gf_free(ptr);
}