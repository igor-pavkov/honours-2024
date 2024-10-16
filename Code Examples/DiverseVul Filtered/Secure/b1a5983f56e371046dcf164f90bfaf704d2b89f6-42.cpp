void nft_offload_set_dependency(struct nft_offload_ctx *ctx,
				enum nft_offload_dep_type type)
{
	ctx->dep.type = type;
}