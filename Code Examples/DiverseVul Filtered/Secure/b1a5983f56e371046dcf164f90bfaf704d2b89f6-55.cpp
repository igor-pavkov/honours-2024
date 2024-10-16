static int nft_block_offload_cmd(struct nft_base_chain *chain,
				 struct net_device *dev,
				 enum flow_block_command cmd)
{
	struct netlink_ext_ack extack = {};
	struct flow_block_offload bo;
	int err;

	nft_flow_block_offload_init(&bo, dev_net(dev), cmd, chain, &extack);

	err = dev->netdev_ops->ndo_setup_tc(dev, TC_SETUP_BLOCK, &bo);
	if (err < 0)
		return err;

	return nft_block_setup(chain, &bo, cmd);
}