static int h2c_send_settings(struct h2c *h2c)
{
	struct buffer *res;
	char buf_data[100]; // enough for 15 settings
	struct buffer buf;
	int ret;

	if (h2c_mux_busy(h2c, NULL)) {
		h2c->flags |= H2_CF_DEM_MBUSY;
		return 0;
	}

	res = h2_get_buf(h2c, &h2c->mbuf);
	if (!res) {
		h2c->flags |= H2_CF_MUX_MALLOC;
		h2c->flags |= H2_CF_DEM_MROOM;
		return 0;
	}

	chunk_init(&buf, buf_data, sizeof(buf_data));
	chunk_memcpy(&buf,
	       "\x00\x00\x00"      /* length    : 0 for now */
	       "\x04\x00"          /* type      : 4 (settings), flags : 0 */
	       "\x00\x00\x00\x00", /* stream ID : 0 */
	       9);

	if (h2_settings_header_table_size != 4096) {
		char str[6] = "\x00\x01"; /* header_table_size */

		write_n32(str + 2, h2_settings_header_table_size);
		chunk_memcat(&buf, str, 6);
	}

	if (h2_settings_initial_window_size != 65535) {
		char str[6] = "\x00\x04"; /* initial_window_size */

		write_n32(str + 2, h2_settings_initial_window_size);
		chunk_memcat(&buf, str, 6);
	}

	if (h2_settings_max_concurrent_streams != 0) {
		char str[6] = "\x00\x03"; /* max_concurrent_streams */

		/* Note: 0 means "unlimited" for haproxy's config but not for
		 * the protocol, so never send this value!
		 */
		write_n32(str + 2, h2_settings_max_concurrent_streams);
		chunk_memcat(&buf, str, 6);
	}

	if (global.tune.bufsize != 16384) {
		char str[6] = "\x00\x05"; /* max_frame_size */

		/* note: similarly we could also emit MAX_HEADER_LIST_SIZE to
		 * match bufsize - rewrite size, but at the moment it seems
		 * that clients don't take care of it.
		 */
		write_n32(str + 2, global.tune.bufsize);
		chunk_memcat(&buf, str, 6);
	}

	h2_set_frame_size(buf.area, buf.data - 9);
	ret = b_istput(res, ist2(buf.area, buf.data));
	if (unlikely(ret <= 0)) {
		if (!ret) {
			h2c->flags |= H2_CF_MUX_MFULL;
			h2c->flags |= H2_CF_DEM_MROOM;
			return 0;
		}
		else {
			h2c_error(h2c, H2_ERR_INTERNAL_ERROR);
			return 0;
		}
	}
	return ret;
}