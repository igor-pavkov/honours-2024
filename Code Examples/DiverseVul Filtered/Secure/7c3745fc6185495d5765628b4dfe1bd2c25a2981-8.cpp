void strbuf_repo_git_path(struct strbuf *sb,
			  const struct repository *repo,
			  const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	do_git_path(repo, NULL, sb, fmt, args);
	va_end(args);
}