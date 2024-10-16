static int nft_immediate_offload_verdict(struct nft_offload_ctx *ctx,
					 struct nft_flow_rule *flow,
					 const struct nft_immediate_expr *priv)
{
	struct flow_action_entry *entry;
	const struct nft_data *data;

	entry = &flow->rule->action.entries[ctx->num_actions++];

	data = &priv->data;
	switch (data->verdict.code) {
	case NF_ACCEPT:
		entry->id = FLOW_ACTION_ACCEPT;
		break;
	case NF_DROP:
		entry->id = FLOW_ACTION_DROP;
		break;
	default:
		return -EOPNOTSUPP;
	}

	return 0;
}