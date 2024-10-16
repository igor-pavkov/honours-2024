static int only_spaces_and_periods(const char *path, size_t len, size_t skip)
{
	if (len < skip)
		return 0;
	len -= skip;
	path += skip;
	while (len-- > 0) {
		char c = *(path++);
		if (c != ' ' && c != '.')
			return 0;
	}
	return 1;
}