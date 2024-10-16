static git_tree_entry *alloc_entry(const char *filename)
{
	git_tree_entry *entry = NULL;
	size_t filename_len = strlen(filename);

	entry = git__malloc(sizeof(git_tree_entry) + filename_len + 1);
	if (!entry)
		return NULL;

	memset(entry, 0x0, sizeof(git_tree_entry));
	memcpy(entry->filename, filename, filename_len);
	entry->filename[filename_len] = 0;
	entry->filename_len = filename_len;

	return entry;
}