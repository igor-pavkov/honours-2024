static int nft_chain_offload_cmd(struct nft_base_chain *basechain,
				 struct net_device *dev,
				 enum flow_block_command cmd)
{
	int err;

	if (dev->netdev_ops->ndo_setup_tc)
		err = nft_block_offload_cmd(basechain, dev, cmd);
	else
		err = nft_indr_block_offload_cmd(basechain, dev, cmd);

	return err;
}