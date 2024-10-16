static void nft_flowtable_hooks_destroy(struct list_head *hook_list)
{
	struct nft_hook *hook, *next;

	list_for_each_entry_safe(hook, next, hook_list, list) {
		list_del_rcu(&hook->list);
		kfree_rcu(hook, rcu);
	}
}