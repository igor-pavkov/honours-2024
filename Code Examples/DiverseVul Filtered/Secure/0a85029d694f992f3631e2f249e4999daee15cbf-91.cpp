void vpcc_box_del(GF_Box *s)
{
	GF_VPConfigurationBox *ptr = (GF_VPConfigurationBox*)s;
	if (ptr->config) gf_odf_vp_cfg_del(ptr->config);
	ptr->config = NULL;
	gf_free(ptr);
}