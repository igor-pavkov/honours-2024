GF_Box *avcc_box_new()
{
	GF_AVCConfigurationBox *tmp = (GF_AVCConfigurationBox *) gf_malloc(sizeof(GF_AVCConfigurationBox));
	if (tmp == NULL) return NULL;
	memset(tmp, 0, sizeof(GF_AVCConfigurationBox));
	tmp->type = GF_ISOM_BOX_TYPE_AVCC;
	return (GF_Box *)tmp;
}