static u32 vidtv_s302m_write_h(struct vidtv_encoder *e, u32 p_sz)
{
	struct vidtv_smpte_s302m_es h = {};
	u32 nbytes = 0;

	/* 2 channels, ident: 0, 16 bits per sample */
	h.bitfield = cpu_to_be32((p_sz << 16));

	nbytes += vidtv_memcpy(e->encoder_buf,
			       e->encoder_buf_offset,
			       e->encoder_buf_sz,
			       &h,
			       sizeof(h));

	e->encoder_buf_offset += nbytes;
	return nbytes;
}