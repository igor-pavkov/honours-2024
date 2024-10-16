int git_tree_entry_cmp(const git_tree_entry *e1, const git_tree_entry *e2)
{
	return entry_sort_cmp(e1, e2);
}