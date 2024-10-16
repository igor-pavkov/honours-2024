int __nft_release_basechain(struct nft_ctx *ctx)
{
	struct nft_rule *rule, *nr;

	if (WARN_ON(!nft_is_base_chain(ctx->chain)))
		return 0;

	nf_tables_unregister_hook(ctx->net, ctx->chain->table, ctx->chain);
	list_for_each_entry_safe(rule, nr, &ctx->chain->rules, list) {
		list_del(&rule->list);
		ctx->chain->use--;
		nf_tables_rule_release(ctx, rule);
	}
	nft_chain_del(ctx->chain);
	ctx->table->use--;
	nf_tables_chain_destroy(ctx);

	return 0;
}