int git_tree_entry_dup(git_tree_entry **dest, const git_tree_entry *source)
{
	size_t total_size;
	git_tree_entry *copy;

	assert(source);

	total_size = sizeof(git_tree_entry) + source->filename_len + 1;

	copy = git__malloc(total_size);
	GITERR_CHECK_ALLOC(copy);

	memcpy(copy, source, total_size);

	*dest = copy;
	return 0;
}