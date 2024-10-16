static int sja1105_spi_transfer(const struct sja1105_private *priv,
				const void *tx, void *rx, int size)
{
	struct spi_device *spi = priv->spidev;
	struct spi_transfer transfer = {
		.tx_buf = tx,
		.rx_buf = rx,
		.len = size,
	};
	struct spi_message msg;
	int rc;

	if (size > SJA1105_SIZE_SPI_TRANSFER_MAX) {
		dev_err(&spi->dev, "SPI message (%d) longer than max of %d\n",
			size, SJA1105_SIZE_SPI_TRANSFER_MAX);
		return -EMSGSIZE;
	}

	spi_message_init(&msg);
	spi_message_add_tail(&transfer, &msg);

	rc = spi_sync(spi, &msg);
	if (rc < 0) {
		dev_err(&spi->dev, "SPI transfer failed: %d\n", rc);
		return rc;
	}

	return rc;
}