static int tree_walk(
	const git_tree *tree,
	git_treewalk_cb callback,
	git_buf *path,
	void *payload,
	bool preorder)
{
	int error = 0;
	size_t i;
	const git_tree_entry *entry;

	git_vector_foreach(&tree->entries, i, entry) {
		if (preorder) {
			error = callback(path->ptr, entry, payload);
			if (error < 0) { /* negative value stops iteration */
				giterr_set_after_callback_function(error, "git_tree_walk");
				break;
			}
			if (error > 0) { /* positive value skips this entry */
				error = 0;
				continue;
			}
		}

		if (git_tree_entry__is_tree(entry)) {
			git_tree *subtree;
			size_t path_len = git_buf_len(path);

			error = git_tree_lookup(&subtree, tree->object.repo, &entry->oid);
			if (error < 0)
				break;

			/* append the next entry to the path */
			git_buf_puts(path, entry->filename);
			git_buf_putc(path, '/');

			if (git_buf_oom(path))
				error = -1;
			else
				error = tree_walk(subtree, callback, path, payload, preorder);

			git_tree_free(subtree);
			if (error != 0)
				break;

			git_buf_truncate(path, path_len);
		}

		if (!preorder) {
			error = callback(path->ptr, entry, payload);
			if (error < 0) { /* negative value stops iteration */
				giterr_set_after_callback_function(error, "git_tree_walk");
				break;
			}
			error = 0;
		}
	}

	return error;
}