static void huft_free_all(STATE_PARAM_ONLY)
{
	huft_free(inflate_codes_tl);
	huft_free(inflate_codes_td);
	inflate_codes_tl = NULL;
	inflate_codes_td = NULL;
}