static void replace_dir(struct strbuf *buf, int len, const char *newdir)
{
	int newlen = strlen(newdir);
	int need_sep = (buf->buf[len] && !is_dir_sep(buf->buf[len])) &&
		!is_dir_sep(newdir[newlen - 1]);
	if (need_sep)
		len--;	 /* keep one char, to be replaced with '/'  */
	strbuf_splice(buf, 0, len, newdir, newlen);
	if (need_sep)
		buf->buf[newlen] = '/';
}