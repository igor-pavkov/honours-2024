static bool on_rebase_pointer(ut64 offset, ut64 decorated_addr, RRebaseCtx *ctx) {
	if (offset < ctx->off) {
		return true;
	}
	if (offset >= ctx->eob) {
		return false;
	}
	ut64 in_buf = offset - ctx->off;
	if (in_buf >= ctx->count || (in_buf + 8) > ctx->count) {
		return false;
	}

	RParsedPointer ptr;
	r_parse_pointer (&ptr, decorated_addr, ctx->obj);

	r_write_le64 (&ctx->buf[in_buf], ptr.address);

	return true;
}