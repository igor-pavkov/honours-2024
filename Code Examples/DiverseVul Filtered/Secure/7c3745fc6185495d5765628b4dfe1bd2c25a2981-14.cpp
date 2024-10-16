static void adjust_git_path(const struct repository *repo,
			    struct strbuf *buf, int git_dir_len)
{
	const char *base = buf->buf + git_dir_len;
	if (is_dir_file(base, "info", "grafts"))
		strbuf_splice(buf, 0, buf->len,
			      repo->graft_file, strlen(repo->graft_file));
	else if (!strcmp(base, "index"))
		strbuf_splice(buf, 0, buf->len,
			      repo->index_file, strlen(repo->index_file));
	else if (dir_prefix(base, "objects"))
		replace_dir(buf, git_dir_len + 7, repo->objectdir);
	else if (git_hooks_path && dir_prefix(base, "hooks"))
		replace_dir(buf, git_dir_len + 5, git_hooks_path);
	else if (repo->different_commondir)
		update_common_dir(buf, git_dir_len, repo->commondir);
}