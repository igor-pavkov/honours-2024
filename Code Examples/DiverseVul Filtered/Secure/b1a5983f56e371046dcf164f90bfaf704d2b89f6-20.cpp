static struct nft_chain *__nft_offload_get_chain(const struct nftables_pernet *nft_net,
						 struct net_device *dev)
{
	struct nft_base_chain *basechain;
	struct nft_hook *hook, *found;
	const struct nft_table *table;
	struct nft_chain *chain;

	list_for_each_entry(table, &nft_net->tables, list) {
		if (table->family != NFPROTO_NETDEV)
			continue;

		list_for_each_entry(chain, &table->chains, list) {
			if (!nft_is_base_chain(chain) ||
			    !(chain->flags & NFT_CHAIN_HW_OFFLOAD))
				continue;

			found = NULL;
			basechain = nft_base_chain(chain);
			list_for_each_entry(hook, &basechain->hook_list, list) {
				if (hook->ops.dev != dev)
					continue;

				found = hook;
				break;
			}
			if (!found)
				continue;

			return chain;
		}
	}

	return NULL;
}