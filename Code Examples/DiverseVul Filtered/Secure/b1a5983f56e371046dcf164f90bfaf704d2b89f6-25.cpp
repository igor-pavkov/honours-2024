static int nft_flow_offload_rule(const struct nft_chain *chain,
				 struct nft_rule *rule,
				 struct nft_flow_rule *flow,
				 enum flow_cls_command command)
{
	struct flow_cls_offload cls_flow;

	return nft_flow_offload_cmd(chain, rule, flow, command, &cls_flow);
}