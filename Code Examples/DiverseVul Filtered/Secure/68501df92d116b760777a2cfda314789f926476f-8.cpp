static int sja1105et_reset_cmd(const void *ctx, const void *data)
{
	const struct sja1105_private *priv = ctx;
	const struct sja1105_reset_cmd *reset = data;
	const struct sja1105_regs *regs = priv->info->regs;
	struct device *dev = priv->ds->dev;
	u8 packed_buf[SJA1105_SIZE_RESET_CMD];

	if (reset->switch_rst ||
	    reset->cfg_rst ||
	    reset->car_rst ||
	    reset->otp_rst ||
	    reset->por_rst) {
		dev_err(dev, "Only warm and cold reset is supported "
			"for SJA1105 E/T!\n");
		return -EINVAL;
	}

	if (reset->warm_rst)
		dev_dbg(dev, "Warm reset requested\n");
	if (reset->cold_rst)
		dev_dbg(dev, "Cold reset requested\n");

	sja1105et_reset_cmd_pack(packed_buf, reset);

	return sja1105_spi_send_packed_buf(priv, SPI_WRITE, regs->rgu,
					   packed_buf, SJA1105_SIZE_RESET_CMD);
}