void hvcc_box_del(GF_Box *s)
{
	GF_HEVCConfigurationBox *ptr = (GF_HEVCConfigurationBox*)s;
	if (ptr->config) gf_odf_hevc_cfg_del(ptr->config);
	gf_free(ptr);
}