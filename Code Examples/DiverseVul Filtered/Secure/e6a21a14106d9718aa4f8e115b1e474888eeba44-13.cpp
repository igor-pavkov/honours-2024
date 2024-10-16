static u32 vidtv_s302m_write_frame(struct vidtv_encoder *e,
				   u16 sample)
{
	struct vidtv_s302m_ctx *ctx = e->ctx;
	struct vidtv_s302m_frame_16 f = {};
	u32 nbytes = 0;

	/* from ffmpeg: see s302enc.c */

	u8 vucf = ctx->frame_index == 0 ? 0x10 : 0;

	f.data[0] = sample & 0xFF;
	f.data[1] = (sample & 0xFF00) >>  8;
	f.data[2] = ((sample & 0x0F)  <<  4) | vucf;
	f.data[3] = (sample & 0x0FF0) >>  4;
	f.data[4] = (sample & 0xF000) >> 12;

	f.data[0] = reverse[f.data[0]];
	f.data[1] = reverse[f.data[1]];
	f.data[2] = reverse[f.data[2]];
	f.data[3] = reverse[f.data[3]];
	f.data[4] = reverse[f.data[4]];

	nbytes += vidtv_memcpy(e->encoder_buf,
			       e->encoder_buf_offset,
			       VIDTV_S302M_BUF_SZ,
			       &f,
			       sizeof(f));

	e->encoder_buf_offset += nbytes;

	ctx->frame_index++;
	if (ctx->frame_index >= S302M_BLOCK_SZ)
		ctx->frame_index = 0;

	return nbytes;
}