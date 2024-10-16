static void nft_flowtable_hook_release(struct nft_flowtable_hook *flowtable_hook)
{
	struct nft_hook *this, *next;

	list_for_each_entry_safe(this, next, &flowtable_hook->list, list) {
		list_del(&this->list);
		kfree(this);
	}
}