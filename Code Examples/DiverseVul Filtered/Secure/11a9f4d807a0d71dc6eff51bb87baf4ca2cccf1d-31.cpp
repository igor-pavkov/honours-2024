char *git_pathdup_submodule(const char *path, const char *fmt, ...)
{
	int err;
	va_list args;
	struct strbuf buf = STRBUF_INIT;
	va_start(args, fmt);
	err = do_submodule_path(&buf, path, fmt, args);
	va_end(args);
	if (err) {
		strbuf_release(&buf);
		return NULL;
	}
	return strbuf_detach(&buf, NULL);
}