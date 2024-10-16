static void naludmx_del_param_list(GF_List *ps, Bool do_free)
{
	if (!ps) return;
	while (gf_list_count(ps)) {
		GF_NALUFFParam *sl = gf_list_pop_back(ps);
		if (sl->data) gf_free(sl->data);
		gf_free(sl);
	}

	if (do_free)
		gf_list_del(ps);
}