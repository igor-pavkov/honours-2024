const git_tree_entry *git_tree_entry_byindex(
	const git_tree *tree, size_t idx)
{
	assert(tree);
	return git_vector_get(&tree->entries, idx);
}