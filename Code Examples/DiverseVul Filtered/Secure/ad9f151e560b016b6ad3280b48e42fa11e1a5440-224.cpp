static bool nft_hook_list_equal(struct list_head *hook_list1,
				struct list_head *hook_list2)
{
	struct nft_hook *hook;
	int n = 0, m = 0;

	n = 0;
	list_for_each_entry(hook, hook_list2, list) {
		if (!nft_hook_list_find(hook_list1, hook))
			return false;

		n++;
	}
	list_for_each_entry(hook, hook_list1, list)
		m++;

	return n == m;
}