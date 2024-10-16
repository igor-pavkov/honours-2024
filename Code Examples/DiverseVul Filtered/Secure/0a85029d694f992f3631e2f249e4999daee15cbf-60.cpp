void av1c_box_del(GF_Box *s) {
	GF_AV1ConfigurationBox *ptr = (GF_AV1ConfigurationBox*)s;
	if (ptr->config) gf_odf_av1_cfg_del(ptr->config);
	gf_free(ptr);
}