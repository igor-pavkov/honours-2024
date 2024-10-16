int git_treebuilder_write(git_oid *oid, git_repository *repo, git_treebuilder *bld)
{
	int error = 0;
	size_t i;
	git_buf tree = GIT_BUF_INIT;
	git_odb *odb;

	assert(bld);

	git_vector_sort(&bld->entries);

	/* Grow the buffer beforehand to an estimated size */
	error = git_buf_grow(&tree, bld->entries.length * 72);

	for (i = 0; i < bld->entries.length && !error; ++i) {
		git_tree_entry *entry = git_vector_get(&bld->entries, i);

		if (entry->removed)
			continue;

		git_buf_printf(&tree, "%o ", entry->attr);
		git_buf_put(&tree, entry->filename, entry->filename_len + 1);
		git_buf_put(&tree, (char *)entry->oid.id, GIT_OID_RAWSZ);

		if (git_buf_oom(&tree))
			error = -1;
	}

	if (!error &&
		!(error = git_repository_odb__weakptr(&odb, repo)))
		error = git_odb_write(oid, odb, tree.ptr, tree.size, GIT_OBJ_TREE);

	git_buf_free(&tree);
	return error;
}