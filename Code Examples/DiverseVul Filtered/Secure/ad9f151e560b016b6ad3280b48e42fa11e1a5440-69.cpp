static int nft_basechain_init(struct nft_base_chain *basechain, u8 family,
			      struct nft_chain_hook *hook, u32 flags)
{
	struct nft_chain *chain;
	struct nft_hook *h;

	basechain->type = hook->type;
	INIT_LIST_HEAD(&basechain->hook_list);
	chain = &basechain->chain;

	if (nft_base_chain_netdev(family, hook->num)) {
		list_splice_init(&hook->list, &basechain->hook_list);
		list_for_each_entry(h, &basechain->hook_list, list)
			nft_basechain_hook_init(&h->ops, family, hook, chain);

		basechain->ops.hooknum	= hook->num;
		basechain->ops.priority	= hook->priority;
	} else {
		nft_basechain_hook_init(&basechain->ops, family, hook, chain);
	}

	chain->flags |= NFT_CHAIN_BASE | flags;
	basechain->policy = NF_ACCEPT;
	if (chain->flags & NFT_CHAIN_HW_OFFLOAD &&
	    nft_chain_offload_priority(basechain) < 0)
		return -EOPNOTSUPP;

	flow_block_init(&basechain->flow_block);

	return 0;
}