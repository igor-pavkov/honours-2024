void csgp_box_del(GF_Box *a)
{
	GF_CompactSampleGroupBox *p = (GF_CompactSampleGroupBox *)a;
	if (p->patterns) {
		u32 i;
		for (i=0; i<p->pattern_count; i++) {
			if (p->patterns[i].sample_group_description_indices)
				gf_free(p->patterns[i].sample_group_description_indices);
		}
		gf_free(p->patterns);
	}
	gf_free(p);
}