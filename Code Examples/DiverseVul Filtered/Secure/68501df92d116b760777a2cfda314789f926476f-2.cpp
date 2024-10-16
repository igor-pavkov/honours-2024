int sja1105_spi_send_packed_buf(const struct sja1105_private *priv,
				sja1105_spi_rw_mode_t rw, u64 reg_addr,
				void *packed_buf, size_t size_bytes)
{
	u8 tx_buf[SJA1105_SIZE_SPI_TRANSFER_MAX] = {0};
	u8 rx_buf[SJA1105_SIZE_SPI_TRANSFER_MAX] = {0};
	const int msg_len = size_bytes + SJA1105_SIZE_SPI_MSG_HEADER;
	struct sja1105_spi_message msg = {0};
	int rc;

	if (msg_len > SJA1105_SIZE_SPI_TRANSFER_MAX)
		return -ERANGE;

	msg.access = rw;
	msg.address = reg_addr;
	if (rw == SPI_READ)
		msg.read_count = size_bytes / 4;

	sja1105_spi_message_pack(tx_buf, &msg);

	if (rw == SPI_WRITE)
		memcpy(tx_buf + SJA1105_SIZE_SPI_MSG_HEADER,
		       packed_buf, size_bytes);

	rc = sja1105_spi_transfer(priv, tx_buf, rx_buf, msg_len);
	if (rc < 0)
		return rc;

	if (rw == SPI_READ)
		memcpy(packed_buf, rx_buf + SJA1105_SIZE_SPI_MSG_HEADER,
		       size_bytes);

	return 0;
}