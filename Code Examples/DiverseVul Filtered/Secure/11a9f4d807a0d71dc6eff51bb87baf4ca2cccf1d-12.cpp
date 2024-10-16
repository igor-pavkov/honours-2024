const char *remove_leading_path(const char *in, const char *prefix)
{
	static struct strbuf buf = STRBUF_INIT;
	int i = 0, j = 0;

	if (!prefix || !prefix[0])
		return in;
	while (prefix[i]) {
		if (is_dir_sep(prefix[i])) {
			if (!is_dir_sep(in[j]))
				return in;
			while (is_dir_sep(prefix[i]))
				i++;
			while (is_dir_sep(in[j]))
				j++;
			continue;
		} else if (in[j] != prefix[i]) {
			return in;
		}
		i++;
		j++;
	}
	if (
	    /* "/foo" is a prefix of "/foo" */
	    in[j] &&
	    /* "/foo" is not a prefix of "/foobar" */
	    !is_dir_sep(prefix[i-1]) && !is_dir_sep(in[j])
	   )
		return in;
	while (is_dir_sep(in[j]))
		j++;

	strbuf_reset(&buf);
	if (!in[j])
		strbuf_addstr(&buf, ".");
	else
		strbuf_addstr(&buf, in + j);
	return buf.buf;
}