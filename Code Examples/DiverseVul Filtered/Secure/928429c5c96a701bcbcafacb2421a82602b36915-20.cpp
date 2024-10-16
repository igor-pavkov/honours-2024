void git_treebuilder_clear(git_treebuilder *bld)
{
	size_t i;
	git_tree_entry *e;

	assert(bld);

	git_vector_foreach(&bld->entries, i, e)
		git_tree_entry_free(e);

	git_vector_clear(&bld->entries);
	bld->entrycount = 0;
}