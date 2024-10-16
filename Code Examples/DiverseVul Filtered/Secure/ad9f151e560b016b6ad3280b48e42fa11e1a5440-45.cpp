static struct nft_hook *nft_hook_list_find(struct list_head *hook_list,
					   const struct nft_hook *this)
{
	struct nft_hook *hook;

	list_for_each_entry(hook, hook_list, list) {
		if (this->ops.dev == hook->ops.dev)
			return hook;
	}

	return NULL;
}