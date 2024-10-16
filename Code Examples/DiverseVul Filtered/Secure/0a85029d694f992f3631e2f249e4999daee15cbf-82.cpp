GF_Box *btrt_box_new()
{
	GF_BitRateBox *tmp = (GF_BitRateBox *) gf_malloc(sizeof(GF_BitRateBox));
	if (tmp == NULL) return NULL;
	memset(tmp, 0, sizeof(GF_BitRateBox));
	tmp->type = GF_ISOM_BOX_TYPE_BTRT;
	return (GF_Box *)tmp;
}