int sja1105_inhibit_tx(const struct sja1105_private *priv,
		       unsigned long port_bitmap, bool tx_inhibited)
{
	const struct sja1105_regs *regs = priv->info->regs;
	u64 inhibit_cmd;
	int rc;

	rc = sja1105_spi_send_int(priv, SPI_READ, regs->port_control,
				  &inhibit_cmd, SJA1105_SIZE_PORT_CTRL);
	if (rc < 0)
		return rc;

	if (tx_inhibited)
		inhibit_cmd |= port_bitmap;
	else
		inhibit_cmd &= ~port_bitmap;

	return sja1105_spi_send_int(priv, SPI_WRITE, regs->port_control,
				    &inhibit_cmd, SJA1105_SIZE_PORT_CTRL);
}