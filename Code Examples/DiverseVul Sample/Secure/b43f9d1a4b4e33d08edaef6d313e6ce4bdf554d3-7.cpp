static void naludmx_push_prefix(GF_NALUDmxCtx *ctx, u8 *data, u32 size, Bool avc_sei_rewrite)
{
	if (ctx->sei_buffer_alloc < ctx->sei_buffer_size + size + ctx->nal_length) {
		ctx->sei_buffer_alloc = ctx->sei_buffer_size + size + ctx->nal_length;
		ctx->sei_buffer = gf_realloc(ctx->sei_buffer, ctx->sei_buffer_alloc);
	}

	if (!ctx->bs_w) ctx->bs_w = gf_bs_new(ctx->sei_buffer + ctx->sei_buffer_size, ctx->nal_length + size, GF_BITSTREAM_WRITE);
	else gf_bs_reassign_buffer(ctx->bs_w, ctx->sei_buffer + ctx->sei_buffer_size, ctx->nal_length + size);
	gf_bs_write_int(ctx->bs_w, size, 8*ctx->nal_length);
	memcpy(ctx->sei_buffer + ctx->sei_buffer_size + ctx->nal_length, data, size);

	if (avc_sei_rewrite) {
		u32 rw_sei_size = gf_avc_reformat_sei(ctx->sei_buffer + ctx->sei_buffer_size + ctx->nal_length, size, ctx->seirw, ctx->avc_state);
		if (rw_sei_size < size) {
			gf_bs_seek(ctx->bs_w, 0);
			gf_bs_write_int(ctx->bs_w, rw_sei_size, 8*ctx->nal_length);
			size = rw_sei_size;
		}
	}
	ctx->sei_buffer_size += size + ctx->nal_length;
}