static void vidtv_s302m_write_frames(struct vidtv_encoder *e)
{
	struct vidtv_access_unit *au = e->access_units;
	struct vidtv_s302m_ctx *ctx = e->ctx;
	u32 nbytes_per_unit = 0;
	u32 nbytes = 0;
	u32 au_sz = 0;
	u16 sample;
	u32 j;

	while (au) {
		au_sz = au->num_samples *
			sizeof(struct vidtv_s302m_frame_16);

		nbytes_per_unit = vidtv_s302m_write_h(e, au_sz);

		for (j = 0; j < au->num_samples; ++j) {
			sample = vidtv_s302m_get_sample(e);
			nbytes_per_unit += vidtv_s302m_write_frame(e, sample);

			if (e->src_buf)
				e->src_buf_offset += sizeof(u16);

			e->sample_count++;
		}

		au->nbytes = nbytes_per_unit;

		if (au_sz + sizeof(struct vidtv_smpte_s302m_es) != nbytes_per_unit) {
			pr_warn_ratelimited("write size was %u, expected %zu\n",
					    nbytes_per_unit,
					    au_sz + sizeof(struct vidtv_smpte_s302m_es));
		}

		nbytes += nbytes_per_unit;
		au->offset = nbytes - nbytes_per_unit;

		nbytes_per_unit = 0;
		ctx->au_count++;

		au = au->next;
	}
}