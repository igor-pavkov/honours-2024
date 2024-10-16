GF_Box *vpcc_box_new()
{
	GF_VPConfigurationBox *tmp = (GF_VPConfigurationBox *) gf_malloc(sizeof(GF_VPConfigurationBox));
	if (tmp == NULL) return NULL;
	memset(tmp, 0, sizeof(GF_VPConfigurationBox));
	tmp->type = GF_ISOM_BOX_TYPE_VPCC;
	tmp->version = 1;
	return (GF_Box *)tmp;
}