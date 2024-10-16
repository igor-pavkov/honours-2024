char *mkpathdup(const char *fmt, ...)
{
	struct strbuf sb = STRBUF_INIT;
	va_list args;
	va_start(args, fmt);
	strbuf_vaddf(&sb, fmt, args);
	va_end(args);
	strbuf_cleanup_path(&sb);
	return strbuf_detach(&sb, NULL);
}