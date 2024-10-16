int git_tree_entry_icmp(const git_tree_entry *e1, const git_tree_entry *e2)
{
	return git_path_cmp(
		e1->filename, e1->filename_len, git_tree_entry__is_tree(e1),
		e2->filename, e2->filename_len, git_tree_entry__is_tree(e2),
		git__strncasecmp);
}