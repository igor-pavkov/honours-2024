static inline unsigned int unimac_mdio_busy(struct unimac_mdio_priv *priv)
{
	return unimac_mdio_readl(priv, MDIO_CMD) & MDIO_START_BUSY;
}