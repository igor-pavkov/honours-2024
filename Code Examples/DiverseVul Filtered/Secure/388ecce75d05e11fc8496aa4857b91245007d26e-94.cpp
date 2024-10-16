void stbl_box_del(GF_Box *s)
{
	GF_SampleTableBox *ptr = (GF_SampleTableBox *)s;
	if (ptr == NULL) return;

	if (ptr->sub_samples) gf_list_del(ptr->sub_samples);
	if (ptr->sampleGroups) gf_list_del(ptr->sampleGroups);
	if (ptr->sampleGroupsDescription) gf_list_del(ptr->sampleGroupsDescription);
	if (ptr->sai_sizes) gf_list_del(ptr->sai_sizes);
	if (ptr->sai_offsets) gf_list_del(ptr->sai_offsets);
	if (ptr->traf_map) {
		if (ptr->traf_map->frag_starts) {
			u32 i;
			for (i=0; i<ptr->traf_map->nb_entries; i++) {
				if (ptr->traf_map->frag_starts[i].moof_template)
					gf_free(ptr->traf_map->frag_starts[i].moof_template);
			}
			gf_free(ptr->traf_map->frag_starts);
		}
		gf_free(ptr->traf_map);
	}
	gf_free(ptr);
}