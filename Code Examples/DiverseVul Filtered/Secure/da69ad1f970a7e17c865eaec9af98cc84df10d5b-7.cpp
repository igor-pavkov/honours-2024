static void flac_dmx_check_dur(GF_Filter *filter, GF_FLACDmxCtx *ctx)
{
	u64 rate;
	FILE *stream;
	const GF_PropertyValue *p;
	if (!ctx->opid || ctx->timescale || ctx->file_loaded) return;

	if (ctx->index<=0) {
		ctx->file_loaded = GF_TRUE;
		return;
	}

	p = gf_filter_pid_get_property(ctx->ipid, GF_PROP_PID_FILEPATH);
	if (!p || !p->value.string || !strncmp(p->value.string, "gmem://", 7)) {
		ctx->is_file = GF_FALSE;
		ctx->file_loaded = GF_TRUE;
		return;
	}
	ctx->is_file = GF_TRUE;

	stream = gf_fopen(p->value.string, "rb");
	if (!stream) return;
	gf_fseek(stream, 0, SEEK_END);

	rate = gf_ftell(stream);
	gf_fclose(stream);
	if (ctx->duration.num && !gf_sys_is_test_mode() ) {
		rate *= 8 * ctx->duration.den;
		rate /= ctx->duration.num;
		ctx->bitrate = (u32) rate;
	}

	p = gf_filter_pid_get_property(ctx->ipid, GF_PROP_PID_FILE_CACHED);
	if (p && p->value.boolean) ctx->file_loaded = GF_TRUE;
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_CAN_DATAREF, & PROP_BOOL(GF_TRUE ) );
}