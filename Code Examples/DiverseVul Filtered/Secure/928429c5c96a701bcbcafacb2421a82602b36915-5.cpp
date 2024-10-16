int git_tree__write_index(
	git_oid *oid, git_index *index, git_repository *repo)
{
	int ret;
	bool old_ignore_case = false;

	assert(oid && index && repo);

	if (git_index_has_conflicts(index)) {
		giterr_set(GITERR_INDEX,
			"Cannot create a tree from a not fully merged index.");
		return GIT_EUNMERGED;
	}

	if (index->tree != NULL && index->tree->entries >= 0) {
		git_oid_cpy(oid, &index->tree->oid);
		return 0;
	}

	/* The tree cache didn't help us; we'll have to write
	 * out a tree. If the index is ignore_case, we must
	 * make it case-sensitive for the duration of the tree-write
	 * operation. */

	if (index->ignore_case) {
		old_ignore_case = true;
		git_index__set_ignore_case(index, false);
	}

	ret = write_tree(oid, repo, index, "", 0);

	if (old_ignore_case)
		git_index__set_ignore_case(index, true);

	return ret < 0 ? ret : 0;
}