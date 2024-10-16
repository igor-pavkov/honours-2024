int get_tree_entry(const unsigned char *tree_sha1, const char *name, unsigned char *sha1, unsigned *mode)
{
	int retval;
	void *tree;
	unsigned long size;
	unsigned char root[20];

	tree = read_object_with_reference(tree_sha1, tree_type, &size, root);
	if (!tree)
		return -1;

	if (name[0] == '\0') {
		hashcpy(sha1, root);
		free(tree);
		return 0;
	}

	if (!size) {
		retval = -1;
	} else {
		struct tree_desc t;
		init_tree_desc(&t, tree, size);
		retval = find_tree_entry(&t, name, sha1, mode);
	}
	free(tree);
	return retval;
}