GF_Box *hvcc_box_new()
{
	GF_HEVCConfigurationBox *tmp = (GF_HEVCConfigurationBox *) gf_malloc(sizeof(GF_HEVCConfigurationBox));
	if (tmp == NULL) return NULL;
	memset(tmp, 0, sizeof(GF_HEVCConfigurationBox));
	tmp->type = GF_ISOM_BOX_TYPE_HVCC;
	return (GF_Box *)tmp;
}