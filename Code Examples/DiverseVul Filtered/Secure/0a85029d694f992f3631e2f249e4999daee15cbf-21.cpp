GF_Box *av1c_box_new() {
	GF_AV1ConfigurationBox *tmp = (GF_AV1ConfigurationBox *)gf_malloc(sizeof(GF_AV1ConfigurationBox));
	if (tmp == NULL) return NULL;
	memset(tmp, 0, sizeof(GF_AV1ConfigurationBox));
	tmp->type = GF_ISOM_BOX_TYPE_AV1C;
	return (GF_Box *)tmp;
}