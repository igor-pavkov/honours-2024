void sbgp_box_del(GF_Box *a)
{
	GF_SampleGroupBox *p = (GF_SampleGroupBox *)a;
	if (p->sample_entries) gf_free(p->sample_entries);
	gf_free(p);
}