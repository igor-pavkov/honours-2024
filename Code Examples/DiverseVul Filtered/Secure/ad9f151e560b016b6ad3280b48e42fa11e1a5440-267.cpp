static void nf_tables_table_disable(struct net *net, struct nft_table *table)
{
	table->flags &= ~NFT_TABLE_F_DORMANT;
	nft_table_disable(net, table, 0);
	table->flags |= NFT_TABLE_F_DORMANT;
}