static int regex_match_glob(char *str, struct regex *r, int len __maybe_unused)
{
	if (glob_match(r->pattern, str))
		return 1;
	return 0;
}