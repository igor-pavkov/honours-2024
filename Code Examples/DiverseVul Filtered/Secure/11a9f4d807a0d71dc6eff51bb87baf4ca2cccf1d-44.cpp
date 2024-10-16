const char *relative_path(const char *in, const char *prefix,
			  struct strbuf *sb)
{
	int in_len = in ? strlen(in) : 0;
	int prefix_len = prefix ? strlen(prefix) : 0;
	int in_off = 0;
	int prefix_off = 0;
	int i = 0, j = 0;

	if (!in_len)
		return "./";
	else if (!prefix_len)
		return in;

	if (have_same_root(in, prefix))
		/* bypass dos_drive, for "c:" is identical to "C:" */
		i = j = has_dos_drive_prefix(in);
	else {
		return in;
	}

	while (i < prefix_len && j < in_len && prefix[i] == in[j]) {
		if (is_dir_sep(prefix[i])) {
			while (is_dir_sep(prefix[i]))
				i++;
			while (is_dir_sep(in[j]))
				j++;
			prefix_off = i;
			in_off = j;
		} else {
			i++;
			j++;
		}
	}

	if (
	    /* "prefix" seems like prefix of "in" */
	    i >= prefix_len &&
	    /*
	     * but "/foo" is not a prefix of "/foobar"
	     * (i.e. prefix not end with '/')
	     */
	    prefix_off < prefix_len) {
		if (j >= in_len) {
			/* in="/a/b", prefix="/a/b" */
			in_off = in_len;
		} else if (is_dir_sep(in[j])) {
			/* in="/a/b/c", prefix="/a/b" */
			while (is_dir_sep(in[j]))
				j++;
			in_off = j;
		} else {
			/* in="/a/bbb/c", prefix="/a/b" */
			i = prefix_off;
		}
	} else if (
		   /* "in" is short than "prefix" */
		   j >= in_len &&
		   /* "in" not end with '/' */
		   in_off < in_len) {
		if (is_dir_sep(prefix[i])) {
			/* in="/a/b", prefix="/a/b/c/" */
			while (is_dir_sep(prefix[i]))
				i++;
			in_off = in_len;
		}
	}
	in += in_off;
	in_len -= in_off;

	if (i >= prefix_len) {
		if (!in_len)
			return "./";
		else
			return in;
	}

	strbuf_reset(sb);
	strbuf_grow(sb, in_len);

	while (i < prefix_len) {
		if (is_dir_sep(prefix[i])) {
			strbuf_addstr(sb, "../");
			while (is_dir_sep(prefix[i]))
				i++;
			continue;
		}
		i++;
	}
	if (!is_dir_sep(prefix[prefix_len - 1]))
		strbuf_addstr(sb, "../");

	strbuf_addstr(sb, in);

	return sb->buf;
}