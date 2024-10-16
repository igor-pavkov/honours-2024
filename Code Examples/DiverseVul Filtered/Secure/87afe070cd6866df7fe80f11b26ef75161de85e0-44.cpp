u32 parse_base_url(char *arg_val, u32 opt)
{
	mpd_base_urls = gf_realloc(mpd_base_urls, (nb_mpd_base_urls + 1)*sizeof(char**));
	if (!mpd_base_urls) return 2;
	mpd_base_urls[nb_mpd_base_urls] = arg_val;
	nb_mpd_base_urls++;
	return 0;
}