void sdtp_box_del(GF_Box *s)
{
	GF_SampleDependencyTypeBox *ptr = (GF_SampleDependencyTypeBox*)s;
	if (ptr == NULL) return;
	if (ptr->sample_info) gf_free(ptr->sample_info);
	gf_free(ptr);
}