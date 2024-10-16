int strbuf_git_path_submodule(struct strbuf *buf, const char *path,
			      const char *fmt, ...)
{
	int err;
	va_list args;
	va_start(args, fmt);
	err = do_submodule_path(buf, path, fmt, args);
	va_end(args);

	return err;
}