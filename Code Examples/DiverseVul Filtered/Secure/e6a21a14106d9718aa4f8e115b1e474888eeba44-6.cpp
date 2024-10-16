static void *vidtv_s302m_encode(struct vidtv_encoder *e)
{
	struct vidtv_s302m_ctx *ctx = e->ctx;

	/*
	 * According to SMPTE 302M, an audio access unit is specified as those
	 * AES3 words that are associated with a corresponding video frame.
	 * Therefore, there is one audio access unit for every video access unit
	 * in the corresponding video encoder ('sync'), using the same values
	 * for PTS as used by the video encoder.
	 *
	 * Assuming that it is also possible to send audio without any
	 * associated video, as in a radio-like service, a single audio access unit
	 * is created with values for 'num_samples' and 'pts' taken empirically from
	 * ffmpeg
	 */

	vidtv_s302m_access_unit_destroy(e);
	vidtv_s302m_alloc_au(e);

	if (e->sync && e->sync->is_video_encoder) {
		vidtv_s302m_compute_sample_count_from_video(e);
		vidtv_s302m_compute_pts_from_video(e);
	} else {
		e->access_units->num_samples = FF_S302M_DEFAULT_NUM_FRAMES;
		e->access_units->pts = (ctx->au_count * FF_S302M_DEFAULT_PTS_INCREMENT) +
				       FF_S302M_DEFAULT_PTS_OFFSET;
	}

	vidtv_s302m_write_frames(e);

	return e->encoder_buf;
}