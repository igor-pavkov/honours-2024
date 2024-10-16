static void extended_entry_extract(struct tree_desc_x *t,
				   struct name_entry *a,
				   const char *first,
				   int first_len)
{
	const char *path;
	int len;
	struct tree_desc probe;
	struct tree_desc_skip *skip;

	/*
	 * Extract the first entry from the tree_desc, but skip the
	 * ones that we already returned in earlier rounds.
	 */
	while (1) {
		if (!t->d.size) {
			entry_clear(a);
			break; /* not found */
		}
		entry_extract(&t->d, a);
		for (skip = t->skip; skip; skip = skip->prev)
			if (a->path == skip->ptr)
				break; /* found */
		if (!skip)
			break;
		/* We have processed this entry already. */
		update_tree_entry(&t->d);
	}

	if (!first || !a->path)
		return;

	/*
	 * The caller wants "first" from this tree, or nothing.
	 */
	path = a->path;
	len = tree_entry_len(a);
	switch (check_entry_match(first, first_len, path, len)) {
	case -1:
		entry_clear(a);
	case 0:
		return;
	default:
		break;
	}

	/*
	 * We need to look-ahead -- we suspect that a subtree whose
	 * name is "first" may be hiding behind the current entry "path".
	 */
	probe = t->d;
	while (probe.size) {
		entry_extract(&probe, a);
		path = a->path;
		len = tree_entry_len(a);
		switch (check_entry_match(first, first_len, path, len)) {
		case -1:
			entry_clear(a);
		case 0:
			return;
		default:
			update_tree_entry(&probe);
			break;
		}
		/* keep looking */
	}
	entry_clear(a);
}