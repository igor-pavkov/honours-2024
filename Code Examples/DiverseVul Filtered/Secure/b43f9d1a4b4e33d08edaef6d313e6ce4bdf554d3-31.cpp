void naludmx_add_subsample(GF_NALUDmxCtx *ctx, u32 subs_size, u8 subs_priority, u32 subs_reserved)
{
	if (ctx->subsamp_buffer_alloc < ctx->subsamp_buffer_size+14 ) {
		ctx->subsamp_buffer_alloc = ctx->subsamp_buffer_size+14;
		ctx->subsamp_buffer = gf_realloc(ctx->subsamp_buffer, ctx->subsamp_buffer_alloc);
	}
	assert(ctx->subsamp_buffer);
	gf_bs_reassign_buffer(ctx->bs_w, ctx->subsamp_buffer + ctx->subsamp_buffer_size, 14);
	gf_bs_write_u32(ctx->bs_w, 0); //flags
	gf_bs_write_u32(ctx->bs_w, subs_size + ctx->nal_length);
	gf_bs_write_u32(ctx->bs_w, subs_reserved); //reserved
	gf_bs_write_u8(ctx->bs_w, subs_priority); //priority
	gf_bs_write_u8(ctx->bs_w, 0); //discardable - todo
	ctx->subsamp_buffer_size += 14;
	ctx->subs_mapped_bytes += subs_size + ctx->nal_length;
}