int nft_flow_rule_stats(const struct nft_chain *chain,
			const struct nft_rule *rule)
{
	struct flow_cls_offload cls_flow = {};
	struct nft_expr *expr, *next;
	int err;

	err = nft_flow_offload_cmd(chain, rule, NULL, FLOW_CLS_STATS,
				   &cls_flow);
	if (err < 0)
		return err;

	nft_rule_for_each_expr(expr, next, rule) {
		if (expr->ops->offload_stats)
			expr->ops->offload_stats(expr, &cls_flow.stats);
	}

	return 0;
}