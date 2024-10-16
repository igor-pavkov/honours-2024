static int nf_tables_validate(struct net *net)
{
	struct nftables_pernet *nft_net = nft_pernet(net);
	struct nft_table *table;

	switch (nft_net->validate_state) {
	case NFT_VALIDATE_SKIP:
		break;
	case NFT_VALIDATE_NEED:
		nft_validate_state_update(net, NFT_VALIDATE_DO);
		fallthrough;
	case NFT_VALIDATE_DO:
		list_for_each_entry(table, &nft_net->tables, list) {
			if (nft_table_validate(net, table) < 0)
				return -EAGAIN;
		}
		break;
	}

	return 0;
}