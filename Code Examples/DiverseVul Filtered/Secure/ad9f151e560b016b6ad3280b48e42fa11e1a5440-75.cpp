static int nf_tables_dump_rules_start(struct netlink_callback *cb)
{
	const struct nlattr * const *nla = cb->data;
	struct nft_rule_dump_ctx *ctx = NULL;

	if (nla[NFTA_RULE_TABLE] || nla[NFTA_RULE_CHAIN]) {
		ctx = kzalloc(sizeof(*ctx), GFP_ATOMIC);
		if (!ctx)
			return -ENOMEM;

		if (nla[NFTA_RULE_TABLE]) {
			ctx->table = nla_strdup(nla[NFTA_RULE_TABLE],
							GFP_ATOMIC);
			if (!ctx->table) {
				kfree(ctx);
				return -ENOMEM;
			}
		}
		if (nla[NFTA_RULE_CHAIN]) {
			ctx->chain = nla_strdup(nla[NFTA_RULE_CHAIN],
						GFP_ATOMIC);
			if (!ctx->chain) {
				kfree(ctx->table);
				kfree(ctx);
				return -ENOMEM;
			}
		}
	}

	cb->data = ctx;
	return 0;
}