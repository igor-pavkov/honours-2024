static void strbuf_worktree_gitdir(struct strbuf *buf,
				   const struct repository *repo,
				   const struct worktree *wt)
{
	if (!wt)
		strbuf_addstr(buf, repo->gitdir);
	else if (!wt->id)
		strbuf_addstr(buf, repo->commondir);
	else
		strbuf_git_common_path(buf, repo, "worktrees/%s", wt->id);
}