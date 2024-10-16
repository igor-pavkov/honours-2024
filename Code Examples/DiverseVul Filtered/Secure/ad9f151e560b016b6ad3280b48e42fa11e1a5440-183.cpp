static void nf_tables_table_destroy(struct nft_ctx *ctx)
{
	if (WARN_ON(ctx->table->use > 0))
		return;

	rhltable_destroy(&ctx->table->chains_ht);
	kfree(ctx->table->name);
	kfree(ctx->table->udata);
	kfree(ctx->table);
}