static u32 vidtv_s302m_clear(struct vidtv_encoder *e)
{
	struct vidtv_access_unit *au = e->access_units;
	u32 count = 0;

	while (au) {
		count++;
		au = au->next;
	}

	vidtv_s302m_access_unit_destroy(e);
	memset(e->encoder_buf, 0, VIDTV_S302M_BUF_SZ);
	e->encoder_buf_offset = 0;

	return count;
}