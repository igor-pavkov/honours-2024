void repo_read_gitmodules(struct repository *repo)
{
	submodule_cache_check_init(repo);

	if (repo->worktree) {
		char *gitmodules;

		if (repo_read_index(repo) < 0)
			return;

		gitmodules = repo_worktree_path(repo, GITMODULES_FILE);

		if (!is_gitmodules_unmerged(repo->index))
			git_config_from_file(gitmodules_cb, gitmodules, repo);

		free(gitmodules);
	}

	repo->submodule_cache->gitmodules_read = 1;
}