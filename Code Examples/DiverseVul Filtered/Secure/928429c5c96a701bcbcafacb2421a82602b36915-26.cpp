void git_tree__free(void *_tree)
{
	git_tree *tree = _tree;
	size_t i;
	git_tree_entry *e;

	git_vector_foreach(&tree->entries, i, e)
		git_tree_entry_free(e);

	git_vector_free(&tree->entries);
	git__free(tree);
}