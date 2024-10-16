const git_tree_entry *git_tree_entry_byname(
	const git_tree *tree, const char *filename)
{
	assert(tree && filename);
	return entry_fromname(tree, filename, strlen(filename));
}