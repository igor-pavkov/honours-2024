void avcc_box_del(GF_Box *s)
{
	GF_AVCConfigurationBox *ptr = (GF_AVCConfigurationBox *)s;
	if (ptr->config) gf_odf_avc_cfg_del(ptr->config);
	ptr->config = NULL;
	gf_free(ptr);
}