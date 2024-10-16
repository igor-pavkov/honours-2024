static int match_dir_prefix(const struct pathspec_item *item,
			    const char *base,
			    const char *match, int matchlen)
{
	if (basecmp(item, base, match, matchlen))
		return 0;

	/*
	 * If the base is a subdirectory of a path which
	 * was specified, all of them are interesting.
	 */
	if (!matchlen ||
	    base[matchlen] == '/' ||
	    match[matchlen - 1] == '/')
		return 1;

	/* Just a random prefix match */
	return 0;
}