void strbuf_repo_worktree_path(struct strbuf *sb,
			       const struct repository *repo,
			       const char *fmt, ...)
{
	va_list args;

	if (!repo->worktree)
		return;

	va_start(args, fmt);
	do_worktree_path(repo, sb, fmt, args);
	va_end(args);
}