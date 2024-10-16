static int nft_flow_offload_chain(struct nft_chain *chain, u8 *ppolicy,
				  enum flow_block_command cmd)
{
	struct nft_base_chain *basechain;
	u8 policy;

	if (!nft_is_base_chain(chain))
		return -EOPNOTSUPP;

	basechain = nft_base_chain(chain);
	policy = ppolicy ? *ppolicy : basechain->policy;

	/* Only default policy to accept is supported for now. */
	if (cmd == FLOW_BLOCK_BIND && policy == NF_DROP)
		return -EOPNOTSUPP;

	return nft_flow_block_chain(basechain, NULL, cmd);
}