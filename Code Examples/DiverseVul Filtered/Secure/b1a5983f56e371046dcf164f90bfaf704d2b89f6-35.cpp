static int nft_flow_block_chain(struct nft_base_chain *basechain,
				const struct net_device *this_dev,
				enum flow_block_command cmd)
{
	struct net_device *dev;
	struct nft_hook *hook;
	int err, i = 0;

	list_for_each_entry(hook, &basechain->hook_list, list) {
		dev = hook->ops.dev;
		if (this_dev && this_dev != dev)
			continue;

		err = nft_chain_offload_cmd(basechain, dev, cmd);
		if (err < 0 && cmd == FLOW_BLOCK_BIND) {
			if (!this_dev)
				goto err_flow_block;

			return err;
		}
		i++;
	}

	return 0;

err_flow_block:
	list_for_each_entry(hook, &basechain->hook_list, list) {
		if (i-- <= 0)
			break;

		dev = hook->ops.dev;
		nft_chain_offload_cmd(basechain, dev, FLOW_BLOCK_UNBIND);
	}
	return err;
}