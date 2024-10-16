static void init_common_trie(void)
{
	struct common_dir *p;

	if (common_trie_done_setup)
		return;

	for (p = common_list; p->dirname; p++)
		add_to_trie(&common_trie, p->dirname, p);

	common_trie_done_setup = 1;
}