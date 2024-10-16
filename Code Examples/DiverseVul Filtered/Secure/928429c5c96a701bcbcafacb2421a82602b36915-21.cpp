const git_tree_entry *git_tree_entry_byid(
	const git_tree *tree, const git_oid *id)
{
	size_t i;
	const git_tree_entry *e;

	assert(tree);

	git_vector_foreach(&tree->entries, i, e) {
		if (memcmp(&e->oid.id, &id->id, sizeof(id->id)) == 0)
			return e;
	}

	return NULL;
}