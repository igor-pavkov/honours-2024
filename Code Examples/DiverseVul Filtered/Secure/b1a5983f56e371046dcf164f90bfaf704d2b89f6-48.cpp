static int nft_indr_block_offload_cmd(struct nft_base_chain *basechain,
				      struct net_device *dev,
				      enum flow_block_command cmd)
{
	struct netlink_ext_ack extack = {};
	struct flow_block_offload bo;
	int err;

	nft_flow_block_offload_init(&bo, dev_net(dev), cmd, basechain, &extack);

	err = flow_indr_dev_setup_offload(dev, NULL, TC_SETUP_BLOCK, basechain, &bo,
					  nft_indr_block_cleanup);
	if (err < 0)
		return err;

	if (list_empty(&bo.cb_list))
		return -EOPNOTSUPP;

	return nft_block_setup(basechain, &bo, cmd);
}