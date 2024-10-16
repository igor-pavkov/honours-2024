int nft_chain_offload_priority(struct nft_base_chain *basechain)
{
	if (basechain->ops.priority <= 0 ||
	    basechain->ops.priority > USHRT_MAX)
		return -1;

	return 0;
}