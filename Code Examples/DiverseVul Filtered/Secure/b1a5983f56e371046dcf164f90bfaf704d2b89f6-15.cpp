static int nft_block_setup(struct nft_base_chain *basechain,
			   struct flow_block_offload *bo,
			   enum flow_block_command cmd)
{
	int err;

	switch (cmd) {
	case FLOW_BLOCK_BIND:
		err = nft_flow_offload_bind(bo, basechain);
		break;
	case FLOW_BLOCK_UNBIND:
		err = nft_flow_offload_unbind(bo, basechain);
		break;
	default:
		WARN_ON_ONCE(1);
		err = -EOPNOTSUPP;
	}

	return err;
}