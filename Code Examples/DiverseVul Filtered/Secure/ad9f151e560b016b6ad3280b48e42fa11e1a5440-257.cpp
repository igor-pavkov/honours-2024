static int nf_tables_table_enable(struct net *net, struct nft_table *table)
{
	struct nft_chain *chain;
	int err, i = 0;

	list_for_each_entry(chain, &table->chains, list) {
		if (!nft_is_active_next(net, chain))
			continue;
		if (!nft_is_base_chain(chain))
			continue;

		err = nf_tables_register_hook(net, table, chain);
		if (err < 0)
			goto err_register_hooks;

		i++;
	}
	return 0;

err_register_hooks:
	if (i)
		nft_table_disable(net, table, i);
	return err;
}