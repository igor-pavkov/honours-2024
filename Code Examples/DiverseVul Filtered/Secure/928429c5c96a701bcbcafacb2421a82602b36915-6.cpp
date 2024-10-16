int git_treebuilder_create(git_treebuilder **builder_p, const git_tree *source)
{
	git_treebuilder *bld;
	size_t i, source_entries = DEFAULT_TREE_SIZE;

	assert(builder_p);

	bld = git__calloc(1, sizeof(git_treebuilder));
	GITERR_CHECK_ALLOC(bld);

	if (source != NULL)
		source_entries = source->entries.length;

	if (git_vector_init(&bld->entries, source_entries, entry_sort_cmp) < 0)
		goto on_error;

	if (source != NULL) {
		git_tree_entry *entry_src;

		git_vector_foreach(&source->entries, i, entry_src) {
			if (append_entry(
				bld, entry_src->filename,
				&entry_src->oid,
				entry_src->attr) < 0)
				goto on_error;
		}
	}

	*builder_p = bld;
	return 0;

on_error:
	git_treebuilder_free(bld);
	return -1;
}