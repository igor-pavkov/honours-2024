void gf_isom_enable_traf_map_templates(GF_ISOFile *movie)
{
	if (movie)
		movie->signal_frag_bounds=GF_TRUE;
}