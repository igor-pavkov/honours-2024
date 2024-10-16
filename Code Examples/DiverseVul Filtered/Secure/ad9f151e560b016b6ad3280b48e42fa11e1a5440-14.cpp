static void nft_chain_commit_update(struct nft_trans *trans)
{
	struct nft_base_chain *basechain;

	if (nft_trans_chain_name(trans)) {
		rhltable_remove(&trans->ctx.table->chains_ht,
				&trans->ctx.chain->rhlhead,
				nft_chain_ht_params);
		swap(trans->ctx.chain->name, nft_trans_chain_name(trans));
		rhltable_insert_key(&trans->ctx.table->chains_ht,
				    trans->ctx.chain->name,
				    &trans->ctx.chain->rhlhead,
				    nft_chain_ht_params);
	}

	if (!nft_is_base_chain(trans->ctx.chain))
		return;

	nft_chain_stats_replace(trans);

	basechain = nft_base_chain(trans->ctx.chain);

	switch (nft_trans_chain_policy(trans)) {
	case NF_DROP:
	case NF_ACCEPT:
		basechain->policy = nft_trans_chain_policy(trans);
		break;
	}
}