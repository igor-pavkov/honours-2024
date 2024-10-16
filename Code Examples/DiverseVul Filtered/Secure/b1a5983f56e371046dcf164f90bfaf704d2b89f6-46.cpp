static void nft_flow_block_offload_init(struct flow_block_offload *bo,
					struct net *net,
					enum flow_block_command cmd,
					struct nft_base_chain *basechain,
					struct netlink_ext_ack *extack)
{
	memset(bo, 0, sizeof(*bo));
	bo->net		= net;
	bo->block	= &basechain->flow_block;
	bo->command	= cmd;
	bo->binder_type	= FLOW_BLOCK_BINDER_TYPE_CLSACT_INGRESS;
	bo->extack	= extack;
	bo->cb_list_head = &basechain->flow_block.cb_list;
	INIT_LIST_HEAD(&bo->cb_list);
}