static void nft_indr_block_cleanup(struct flow_block_cb *block_cb)
{
	struct nft_base_chain *basechain = block_cb->indr.data;
	struct net_device *dev = block_cb->indr.dev;
	struct netlink_ext_ack extack = {};
	struct nftables_pernet *nft_net;
	struct net *net = dev_net(dev);
	struct flow_block_offload bo;

	nft_flow_block_offload_init(&bo, dev_net(dev), FLOW_BLOCK_UNBIND,
				    basechain, &extack);
	nft_net = nft_pernet(net);
	mutex_lock(&nft_net->commit_mutex);
	list_del(&block_cb->driver_list);
	list_move(&block_cb->list, &bo.cb_list);
	nft_flow_offload_unbind(&bo, basechain);
	mutex_unlock(&nft_net->commit_mutex);
}