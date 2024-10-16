static u16 vidtv_s302m_get_sample(struct vidtv_encoder *e)
{
	u16 sample;
	int pos;
	struct vidtv_s302m_ctx *ctx = e->ctx;

	if (!e->src_buf) {
		/*
		 * Simple tone generator: play the tones at the
		 * beethoven_fur_elise array.
		 */
		if (ctx->last_duration <= 0) {
			if (e->src_buf_offset >= ARRAY_SIZE(beethoven_fur_elise))
				e->src_buf_offset = 0;

			ctx->last_tone = beethoven_fur_elise[e->src_buf_offset].note;
			ctx->last_duration = beethoven_fur_elise[e->src_buf_offset].duration *
					     S302M_SAMPLING_RATE_HZ / COMPASS / 5;
			e->src_buf_offset++;
			ctx->note_offset = 0;
		} else {
			ctx->last_duration--;
		}

		/* Handle pause notes */
		if (!ctx->last_tone)
			return 0x8000;

		pos = (2 * PI * ctx->note_offset * ctx->last_tone) / S302M_SAMPLING_RATE_HZ;
		ctx->note_offset++;

		return (fixp_sin32(pos % (2 * PI)) >> 16) + 0x8000;
	}

	/* bug somewhere */
	if (e->src_buf_offset > e->src_buf_sz) {
		pr_err_ratelimited("overflow detected: %d > %d, wrapping.\n",
				   e->src_buf_offset,
				   e->src_buf_sz);

		e->src_buf_offset = 0;
	}

	if (e->src_buf_offset >= e->src_buf_sz) {
		/* let the source know we are out of data */
		if (e->last_sample_cb)
			e->last_sample_cb(e->sample_count);

		e->src_buf_offset = 0;
	}

	sample = *(u16 *)(e->src_buf + e->src_buf_offset);

	return sample;
}