static void nft_chain_stats_replace(struct nft_trans *trans)
{
	struct nft_base_chain *chain = nft_base_chain(trans->ctx.chain);

	if (!nft_trans_chain_stats(trans))
		return;

	nft_trans_chain_stats(trans) =
		rcu_replace_pointer(chain->stats, nft_trans_chain_stats(trans),
				    lockdep_commit_lock_is_held(trans->ctx.net));

	if (!nft_trans_chain_stats(trans))
		static_branch_inc(&nft_counters_enabled);
}