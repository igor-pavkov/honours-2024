int traverse_trees(int n, struct tree_desc *t, struct traverse_info *info)
{
	int error = 0;
	struct name_entry *entry = xmalloc(n*sizeof(*entry));
	int i;
	struct tree_desc_x *tx = xcalloc(n, sizeof(*tx));
	struct strbuf base = STRBUF_INIT;
	int interesting = 1;
	char *traverse_path;

	for (i = 0; i < n; i++)
		tx[i].d = t[i];

	if (info->prev) {
		strbuf_grow(&base, info->pathlen);
		make_traverse_path(base.buf, info->prev, &info->name);
		base.buf[info->pathlen-1] = '/';
		strbuf_setlen(&base, info->pathlen);
		traverse_path = xstrndup(base.buf, info->pathlen);
	} else {
		traverse_path = xstrndup(info->name.path, info->pathlen);
	}
	info->traverse_path = traverse_path;
	for (;;) {
		int trees_used;
		unsigned long mask, dirmask;
		const char *first = NULL;
		int first_len = 0;
		struct name_entry *e = NULL;
		int len;

		for (i = 0; i < n; i++) {
			e = entry + i;
			extended_entry_extract(tx + i, e, NULL, 0);
		}

		/*
		 * A tree may have "t-2" at the current location even
		 * though it may have "t" that is a subtree behind it,
		 * and another tree may return "t".  We want to grab
		 * all "t" from all trees to match in such a case.
		 */
		for (i = 0; i < n; i++) {
			e = entry + i;
			if (!e->path)
				continue;
			len = tree_entry_len(e);
			if (!first) {
				first = e->path;
				first_len = len;
				continue;
			}
			if (name_compare(e->path, len, first, first_len) < 0) {
				first = e->path;
				first_len = len;
			}
		}

		if (first) {
			for (i = 0; i < n; i++) {
				e = entry + i;
				extended_entry_extract(tx + i, e, first, first_len);
				/* Cull the ones that are not the earliest */
				if (!e->path)
					continue;
				len = tree_entry_len(e);
				if (name_compare(e->path, len, first, first_len))
					entry_clear(e);
			}
		}

		/* Now we have in entry[i] the earliest name from the trees */
		mask = 0;
		dirmask = 0;
		for (i = 0; i < n; i++) {
			if (!entry[i].path)
				continue;
			mask |= 1ul << i;
			if (S_ISDIR(entry[i].mode))
				dirmask |= 1ul << i;
			e = &entry[i];
		}
		if (!mask)
			break;
		interesting = prune_traversal(e, info, &base, interesting);
		if (interesting < 0)
			break;
		if (interesting) {
			trees_used = info->fn(n, mask, dirmask, entry, info);
			if (trees_used < 0) {
				error = trees_used;
				if (!info->show_all_errors)
					break;
			}
			mask &= trees_used;
		}
		for (i = 0; i < n; i++)
			if (mask & (1ul << i))
				update_extended_entry(tx + i, entry + i);
	}
	free(entry);
	for (i = 0; i < n; i++)
		free_extended_entry(tx + i);
	free(tx);
	free(traverse_path);
	info->traverse_path = NULL;
	strbuf_release(&base);
	return error;
}