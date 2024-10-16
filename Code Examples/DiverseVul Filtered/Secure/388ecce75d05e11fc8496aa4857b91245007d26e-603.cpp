void traf_box_del(GF_Box *s)
{
	GF_TrackFragmentBox *ptr = (GF_TrackFragmentBox *)s;
	if (ptr == NULL) return;
	if (ptr->sub_samples) gf_list_del(ptr->sub_samples);
	gf_list_del(ptr->TrackRuns);
	if (ptr->sampleGroups) gf_list_del(ptr->sampleGroups);
	if (ptr->sampleGroupsDescription) gf_list_del(ptr->sampleGroupsDescription);
	if (ptr->sai_sizes) gf_list_del(ptr->sai_sizes);
	if (ptr->sai_offsets) gf_list_del(ptr->sai_offsets);
	gf_free(ptr);
}