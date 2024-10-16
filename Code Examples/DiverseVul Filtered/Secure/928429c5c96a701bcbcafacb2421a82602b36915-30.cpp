int git_tree__prefix_position(const git_tree *tree, const char *path)
{
	const git_vector *entries = &tree->entries;
	struct tree_key_search ksearch;
	size_t at_pos;

	if (!path)
		return 0;

	ksearch.filename = path;
	ksearch.filename_len = strlen(path);

	/* be safe when we cast away constness - i.e. don't trigger a sort */
	assert(git_vector_is_sorted(&tree->entries));

	/* Find tree entry with appropriate prefix */
	git_vector_bsearch2(
		&at_pos, (git_vector *)entries, &homing_search_cmp, &ksearch);

	for (; at_pos < entries->length; ++at_pos) {
		const git_tree_entry *entry = entries->contents[at_pos];
		if (homing_search_cmp(&ksearch, entry) < 0)
			break;
	}

	for (; at_pos > 0; --at_pos) {
		const git_tree_entry *entry = entries->contents[at_pos - 1];
		if (homing_search_cmp(&ksearch, entry) > 0)
			break;
	}

	return (int)at_pos;
}