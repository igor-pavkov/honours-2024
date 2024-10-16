git_filemode_t git_tree_entry_filemode_raw(const git_tree_entry *entry)
{
	return entry->attr;
}