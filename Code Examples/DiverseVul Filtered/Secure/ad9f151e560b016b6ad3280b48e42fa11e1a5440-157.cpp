static void __nft_release_hooks(struct net *net)
{
	struct nftables_pernet *nft_net = nft_pernet(net);
	struct nft_table *table;

	list_for_each_entry(table, &nft_net->tables, list) {
		if (nft_table_has_owner(table))
			continue;

		__nft_release_hook(net, table);
	}
}