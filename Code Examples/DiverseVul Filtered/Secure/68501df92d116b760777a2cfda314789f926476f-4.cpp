int sja1105_spi_send_long_packed_buf(const struct sja1105_private *priv,
				     sja1105_spi_rw_mode_t rw, u64 base_addr,
				     void *packed_buf, u64 buf_len)
{
	struct chunk {
		void *buf_ptr;
		int len;
		u64 spi_address;
	} chunk;
	int distance_to_end;
	int rc;

	/* Initialize chunk */
	chunk.buf_ptr = packed_buf;
	chunk.spi_address = base_addr;
	chunk.len = min_t(int, buf_len, SJA1105_SIZE_SPI_MSG_MAXLEN);

	while (chunk.len) {
		rc = sja1105_spi_send_packed_buf(priv, rw, chunk.spi_address,
						 chunk.buf_ptr, chunk.len);
		if (rc < 0)
			return rc;

		chunk.buf_ptr += chunk.len;
		chunk.spi_address += chunk.len / 4;
		distance_to_end = (uintptr_t)(packed_buf + buf_len -
					      chunk.buf_ptr);
		chunk.len = min(distance_to_end, SJA1105_SIZE_SPI_MSG_MAXLEN);
	}

	return 0;
}