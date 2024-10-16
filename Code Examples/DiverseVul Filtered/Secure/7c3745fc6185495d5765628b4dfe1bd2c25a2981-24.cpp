void strbuf_git_common_path(struct strbuf *sb,
			    const struct repository *repo,
			    const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	do_git_common_path(repo, sb, fmt, args);
	va_end(args);
}