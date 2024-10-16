void strbuf_git_path(struct strbuf *sb, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	do_git_path(the_repository, NULL, sb, fmt, args);
	va_end(args);
}