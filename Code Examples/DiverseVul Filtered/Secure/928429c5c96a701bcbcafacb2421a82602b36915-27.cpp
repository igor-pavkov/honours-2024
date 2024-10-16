static int tree_key_search(
	size_t *at_pos, git_vector *entries, const char *filename, size_t filename_len)
{
	struct tree_key_search ksearch;
	const git_tree_entry *entry;
	size_t homing, i;

	ksearch.filename = filename;
	ksearch.filename_len = filename_len;

	/* Initial homing search; find an entry on the tree with
	 * the same prefix as the filename we're looking for */
	if (git_vector_bsearch2(&homing, entries, &homing_search_cmp, &ksearch) < 0)
		return GIT_ENOTFOUND; /* just a signal error; not passed back to user */

	/* We found a common prefix. Look forward as long as
	 * there are entries that share the common prefix */
	for (i = homing; i < entries->length; ++i) {
		entry = entries->contents[i];

		if (homing_search_cmp(&ksearch, entry) < 0)
			break;

		if (entry->filename_len == filename_len &&
			memcmp(filename, entry->filename, filename_len) == 0) {
			if (at_pos)
				*at_pos = i;

			return 0;
		}
	}

	/* If we haven't found our filename yet, look backwards
	 * too as long as we have entries with the same prefix */
	if (homing > 0) {
		i = homing - 1;

		do {
			entry = entries->contents[i];

			if (homing_search_cmp(&ksearch, entry) > 0)
				break;

			if (entry->filename_len == filename_len &&
				memcmp(filename, entry->filename, filename_len) == 0) {
				if (at_pos)
					*at_pos = i;

				return 0;
			}
		} while (i-- > 0);
	}

	/* The filename doesn't exist at all */
	return GIT_ENOTFOUND;
}