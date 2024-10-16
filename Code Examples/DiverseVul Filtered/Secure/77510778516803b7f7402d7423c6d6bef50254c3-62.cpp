GF_Box *traf_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_TrackFragmentBox, GF_ISOM_BOX_TYPE_TRAF);
	tmp->TrackRuns = gf_list_new();

	if (gf_sys_old_arch_compat())
		tmp->no_sdtp_first_flags = 1;
	return (GF_Box *)tmp;
}