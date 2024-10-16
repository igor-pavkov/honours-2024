size_t git_tree_entrycount(const git_tree *tree)
{
	assert(tree);
	return tree->entries.length;
}