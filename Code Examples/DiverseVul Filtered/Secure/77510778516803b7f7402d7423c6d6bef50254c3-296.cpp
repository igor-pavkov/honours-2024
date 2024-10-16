GF_Err sdtp_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_SampleDependencyTypeBox *ptr = (GF_SampleDependencyTypeBox *)s;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_data(bs, (char*)ptr->sample_info, ptr->sampleCount);
	return GF_OK;
}