static void nft_flowtable_hooks_del(struct nft_flowtable *flowtable,
				    struct list_head *hook_list)
{
	struct nft_hook *hook, *next;

	list_for_each_entry_safe(hook, next, &flowtable->hook_list, list) {
		if (hook->inactive)
			list_move(&hook->list, hook_list);
	}
}