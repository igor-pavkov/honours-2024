void saiz_box_del(GF_Box *s)
{
	GF_SampleAuxiliaryInfoSizeBox*ptr = (GF_SampleAuxiliaryInfoSizeBox*)s;
	if (ptr == NULL) return;
	if (ptr->sample_info_size) gf_free(ptr->sample_info_size);
	gf_free(ptr);
}