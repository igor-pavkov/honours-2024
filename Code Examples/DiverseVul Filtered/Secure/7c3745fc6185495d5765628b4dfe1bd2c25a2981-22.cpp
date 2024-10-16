char *repo_worktree_path(const struct repository *repo, const char *fmt, ...)
{
	struct strbuf path = STRBUF_INIT;
	va_list args;

	if (!repo->worktree)
		return NULL;

	va_start(args, fmt);
	do_worktree_path(repo, &path, fmt, args);
	va_end(args);

	return strbuf_detach(&path, NULL);
}