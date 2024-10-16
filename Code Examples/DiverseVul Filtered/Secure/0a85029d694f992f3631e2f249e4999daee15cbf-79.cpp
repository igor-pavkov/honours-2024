void vvcc_box_del(GF_Box *s)
{
	GF_VVCConfigurationBox *ptr = (GF_VVCConfigurationBox*)s;
	if (ptr->config) gf_odf_vvc_cfg_del(ptr->config);
	gf_free(ptr);
}