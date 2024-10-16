static int nft_delrule(struct nft_ctx *ctx, struct nft_rule *rule)
{
	struct nft_flow_rule *flow;
	struct nft_trans *trans;
	int err;

	trans = nft_trans_rule_add(ctx, NFT_MSG_DELRULE, rule);
	if (trans == NULL)
		return -ENOMEM;

	if (ctx->chain->flags & NFT_CHAIN_HW_OFFLOAD) {
		flow = nft_flow_rule_create(ctx->net, rule);
		if (IS_ERR(flow)) {
			nft_trans_destroy(trans);
			return PTR_ERR(flow);
		}

		nft_trans_flow_rule(trans) = flow;
	}

	err = nf_tables_delrule_deactivate(ctx, rule);
	if (err < 0) {
		nft_trans_destroy(trans);
		return err;
	}
	nft_rule_expr_deactivate(ctx, rule, NFT_TRANS_PREPARE);

	return 0;
}