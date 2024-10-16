static void nft_chain_release_hook(struct nft_chain_hook *hook)
{
	struct nft_hook *h, *next;

	list_for_each_entry_safe(h, next, &hook->list, list) {
		list_del(&h->list);
		kfree(h);
	}
	module_put(hook->type->owner);
}