static const git_tree_entry *entry_fromname(
	const git_tree *tree, const char *name, size_t name_len)
{
	size_t idx;

	/* be safe when we cast away constness - i.e. don't trigger a sort */
	assert(git_vector_is_sorted(&tree->entries));

	if (tree_key_search(&idx, (git_vector *)&tree->entries, name, name_len) < 0)
		return NULL;

	return git_vector_get(&tree->entries, idx);
}