int git_tree__parse(void *_tree, git_odb_object *odb_obj)
{
	git_tree *tree = _tree;
	const char *buffer = git_odb_object_data(odb_obj);
	const char *buffer_end = buffer + git_odb_object_size(odb_obj);

	if (git_vector_init(&tree->entries, DEFAULT_TREE_SIZE, entry_sort_cmp) < 0)
		return -1;

	while (buffer < buffer_end) {
		git_tree_entry *entry;
		int attr;

		if (git__strtol32(&attr, buffer, &buffer, 8) < 0 || !buffer)
			return tree_error("Failed to parse tree. Can't parse filemode", NULL);

		if (*buffer++ != ' ')
			return tree_error("Failed to parse tree. Object is corrupted", NULL);

		if (memchr(buffer, 0, buffer_end - buffer) == NULL)
			return tree_error("Failed to parse tree. Object is corrupted", NULL);

		/** Allocate the entry and store it in the entries vector */
		{
			entry = alloc_entry(buffer);
			GITERR_CHECK_ALLOC(entry);

			if (git_vector_insert(&tree->entries, entry) < 0) {
				git__free(entry);
				return -1;
			}

			entry->attr = attr;
		}

		while (buffer < buffer_end && *buffer != 0)
			buffer++;

		buffer++;

		git_oid_fromraw(&entry->oid, (const unsigned char *)buffer);
		buffer += GIT_OID_RAWSZ;
	}

	git_vector_sort(&tree->entries);

	return 0;
}