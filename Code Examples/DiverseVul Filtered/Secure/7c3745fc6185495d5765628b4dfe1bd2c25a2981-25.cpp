static void do_git_path(const struct repository *repo,
			const struct worktree *wt, struct strbuf *buf,
			const char *fmt, va_list args)
{
	int gitdir_len;
	strbuf_worktree_gitdir(buf, repo, wt);
	if (buf->len && !is_dir_sep(buf->buf[buf->len - 1]))
		strbuf_addch(buf, '/');
	gitdir_len = buf->len;
	strbuf_vaddf(buf, fmt, args);
	if (!wt)
		adjust_git_path(repo, buf, gitdir_len);
	strbuf_cleanup_path(buf);
}