void sgpd_box_del(GF_Box *a)
{
	GF_SampleGroupDescriptionBox *p = (GF_SampleGroupDescriptionBox *)a;
	while (gf_list_count(p->group_descriptions)) {
		void *ptr = gf_list_last(p->group_descriptions);
		sgpd_del_entry(p->grouping_type, ptr);
		gf_list_rem_last(p->group_descriptions);
	}
	gf_list_del(p->group_descriptions);
	gf_free(p);
}