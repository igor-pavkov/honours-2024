void saio_box_del(GF_Box *s)
{
	GF_SampleAuxiliaryInfoOffsetBox *ptr = (GF_SampleAuxiliaryInfoOffsetBox*)s;
	if (ptr == NULL) return;
	if (ptr->offsets) gf_free(ptr->offsets);
	if (ptr->cached_data) gf_free(ptr->cached_data);
	gf_free(ptr);
}