static int nft_flow_offload_cmd(const struct nft_chain *chain,
				const struct nft_rule *rule,
				struct nft_flow_rule *flow,
				enum flow_cls_command command,
				struct flow_cls_offload *cls_flow)
{
	struct netlink_ext_ack extack = {};
	struct nft_base_chain *basechain;

	if (!nft_is_base_chain(chain))
		return -EOPNOTSUPP;

	basechain = nft_base_chain(chain);
	nft_flow_cls_offload_setup(cls_flow, basechain, rule, flow, &extack,
				   command);

	return nft_setup_cb_call(TC_SETUP_CLSFLOWER, cls_flow,
				 &basechain->flow_block.cb_list);
}