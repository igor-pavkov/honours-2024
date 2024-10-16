int git_treebuilder_remove(git_treebuilder *bld, const char *filename)
{
	git_tree_entry *remove_ptr = treebuilder_get(bld, filename);

	if (remove_ptr == NULL || remove_ptr->removed)
		return tree_error("Failed to remove entry. File isn't in the tree", filename);

	remove_ptr->removed = 1;
	bld->entrycount--;
	return 0;
}