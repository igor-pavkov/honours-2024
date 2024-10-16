GF_Err sgpd_box_size(GF_Box *s)
{
	u32 i, nb_descs;
	Bool use_def_size = GF_TRUE;
	GF_SampleGroupDescriptionBox *p = (GF_SampleGroupDescriptionBox *)s;

	p->size += 8;

	//we force all sample groups to version 1, v0 being deprecated
	if (!p->version)
		p->version = 1;
	p->size += 4;

	if (p->version>=2)
		p->size += 4;
	p->default_length = 0;

	nb_descs = gf_list_count(p->group_descriptions);
	for (i=0; i<nb_descs; i++) {
		void *ptr = gf_list_get(p->group_descriptions, i);
		u32 size = sgpd_size_entry(p->grouping_type, ptr);
		p->size += size;
		if (use_def_size && !p->default_length) {
			p->default_length = size;
		} else if (p->default_length != size) {
			use_def_size = GF_FALSE;
			p->default_length = 0;
		}
	}
	if (p->version>=1) {
		if (!p->default_length) p->size += nb_descs * 4;
	}
	return GF_OK;
}