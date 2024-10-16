static int sja1105pqrs_reset_cmd(const void *ctx, const void *data)
{
	const struct sja1105_private *priv = ctx;
	const struct sja1105_reset_cmd *reset = data;
	const struct sja1105_regs *regs = priv->info->regs;
	struct device *dev = priv->ds->dev;
	u8 packed_buf[SJA1105_SIZE_RESET_CMD];

	if (reset->switch_rst)
		dev_dbg(dev, "Main reset for all functional modules requested\n");
	if (reset->cfg_rst)
		dev_dbg(dev, "Chip configuration reset requested\n");
	if (reset->car_rst)
		dev_dbg(dev, "Clock and reset control logic reset requested\n");
	if (reset->otp_rst)
		dev_dbg(dev, "OTP read cycle for reading product "
			"config settings requested\n");
	if (reset->warm_rst)
		dev_dbg(dev, "Warm reset requested\n");
	if (reset->cold_rst)
		dev_dbg(dev, "Cold reset requested\n");
	if (reset->por_rst)
		dev_dbg(dev, "Power-on reset requested\n");

	sja1105pqrs_reset_cmd_pack(packed_buf, reset);

	return sja1105_spi_send_packed_buf(priv, SPI_WRITE, regs->rgu,
					   packed_buf, SJA1105_SIZE_RESET_CMD);
}