static int sja1105_cold_reset(const struct sja1105_private *priv)
{
	struct sja1105_reset_cmd reset = {0};

	reset.cold_rst = 1;
	return priv->info->reset_cmd(priv, &reset);
}