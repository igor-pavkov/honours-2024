static void nft_flow_cls_offload_setup(struct flow_cls_offload *cls_flow,
				       const struct nft_base_chain *basechain,
				       const struct nft_rule *rule,
				       const struct nft_flow_rule *flow,
				       struct netlink_ext_ack *extack,
				       enum flow_cls_command command)
{
	__be16 proto = ETH_P_ALL;

	memset(cls_flow, 0, sizeof(*cls_flow));

	if (flow)
		proto = flow->proto;

	nft_flow_offload_common_init(&cls_flow->common, proto,
				     basechain->ops.priority, extack);
	cls_flow->command = command;
	cls_flow->cookie = (unsigned long) rule;
	if (flow)
		cls_flow->rule = flow->rule;
}