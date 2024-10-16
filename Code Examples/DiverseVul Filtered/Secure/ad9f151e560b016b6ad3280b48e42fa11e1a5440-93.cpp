static void nf_tables_unregister_hook(struct net *net,
				      const struct nft_table *table,
				      struct nft_chain *chain)
{
	struct nft_base_chain *basechain;
	const struct nf_hook_ops *ops;

	if (table->flags & NFT_TABLE_F_DORMANT ||
	    !nft_is_base_chain(chain))
		return;
	basechain = nft_base_chain(chain);
	ops = &basechain->ops;

	if (basechain->type->ops_unregister)
		return basechain->type->ops_unregister(net, ops);

	if (nft_base_chain_netdev(table->family, basechain->ops.hooknum))
		nft_netdev_unregister_hooks(net, &basechain->hook_list);
	else
		nf_unregister_net_hook(net, &basechain->ops);
}