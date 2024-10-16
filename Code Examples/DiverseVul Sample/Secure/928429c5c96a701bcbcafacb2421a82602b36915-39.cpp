static git_tree_entry *treebuilder_get(git_treebuilder *bld, const char *filename)
{
	size_t idx;
	git_tree_entry *entry;

	assert(bld && filename);

	if (tree_key_search(&idx, &bld->entries, filename, strlen(filename)) < 0)
		return NULL;

	entry = git_vector_get(&bld->entries, idx);
	if (entry->removed)
		return NULL;

	return entry;
}