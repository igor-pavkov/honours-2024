static int nft_offload_netdev_event(struct notifier_block *this,
				    unsigned long event, void *ptr)
{
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);
	struct nftables_pernet *nft_net;
	struct net *net = dev_net(dev);
	struct nft_chain *chain;

	if (event != NETDEV_UNREGISTER)
		return NOTIFY_DONE;

	nft_net = nft_pernet(net);
	mutex_lock(&nft_net->commit_mutex);
	chain = __nft_offload_get_chain(nft_net, dev);
	if (chain)
		nft_flow_block_chain(nft_base_chain(chain), dev,
				     FLOW_BLOCK_UNBIND);

	mutex_unlock(&nft_net->commit_mutex);

	return NOTIFY_DONE;
}