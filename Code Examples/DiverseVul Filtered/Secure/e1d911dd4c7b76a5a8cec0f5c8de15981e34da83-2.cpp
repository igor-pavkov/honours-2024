static int basecmp(const struct pathspec_item *item,
		   const char *base, const char *match, int len)
{
	if (item->magic & PATHSPEC_ICASE) {
		int ret, n = len > item->prefix ? item->prefix : len;
		ret = strncmp(base, match, n);
		if (ret)
			return ret;
		base += n;
		match += n;
		len -= n;
	}
	return ps_strncmp(item, base, match, len);
}