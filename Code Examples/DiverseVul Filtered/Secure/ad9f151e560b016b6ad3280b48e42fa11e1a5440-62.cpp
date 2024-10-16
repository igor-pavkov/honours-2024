void nft_chain_del(struct nft_chain *chain)
{
	struct nft_table *table = chain->table;

	WARN_ON_ONCE(rhltable_remove(&table->chains_ht, &chain->rhlhead,
				     nft_chain_ht_params));
	list_del_rcu(&chain->list);
}