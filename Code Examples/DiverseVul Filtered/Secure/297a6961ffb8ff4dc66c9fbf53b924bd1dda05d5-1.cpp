static inline void unimac_mdio_start(struct unimac_mdio_priv *priv)
{
	u32 reg;

	reg = unimac_mdio_readl(priv, MDIO_CMD);
	reg |= MDIO_START_BUSY;
	unimac_mdio_writel(priv, reg, MDIO_CMD);
}