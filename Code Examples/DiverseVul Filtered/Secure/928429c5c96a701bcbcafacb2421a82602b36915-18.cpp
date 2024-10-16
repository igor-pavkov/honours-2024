git_filemode_t git_tree_entry_filemode(const git_tree_entry *entry)
{
	return normalize_filemode(entry->attr);
}