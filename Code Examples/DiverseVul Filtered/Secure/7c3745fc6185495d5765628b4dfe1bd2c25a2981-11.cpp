static void do_git_common_path(const struct repository *repo,
			       struct strbuf *buf,
			       const char *fmt,
			       va_list args)
{
	strbuf_addstr(buf, repo->commondir);
	if (buf->len && !is_dir_sep(buf->buf[buf->len - 1]))
		strbuf_addch(buf, '/');
	strbuf_vaddf(buf, fmt, args);
	strbuf_cleanup_path(buf);
}