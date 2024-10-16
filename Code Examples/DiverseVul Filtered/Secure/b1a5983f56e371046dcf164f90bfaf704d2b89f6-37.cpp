static int nft_flow_offload_bind(struct flow_block_offload *bo,
				 struct nft_base_chain *basechain)
{
	list_splice(&bo->cb_list, &basechain->flow_block.cb_list);
	return 0;
}