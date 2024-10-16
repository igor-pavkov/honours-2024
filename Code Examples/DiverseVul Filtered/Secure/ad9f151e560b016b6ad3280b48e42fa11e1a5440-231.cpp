static void nft_chain_commit_drop_policy(struct nft_trans *trans)
{
	struct nft_base_chain *basechain;

	if (nft_trans_chain_policy(trans) != NF_DROP)
		return;

	if (!nft_is_base_chain(trans->ctx.chain))
		return;

	basechain = nft_base_chain(trans->ctx.chain);
	basechain->policy = NF_DROP;
}