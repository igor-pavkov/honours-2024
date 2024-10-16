static inline void unimac_mdio_writel(struct unimac_mdio_priv *priv, u32 val,
				      u32 offset)
{
	if (IS_ENABLED(CONFIG_MIPS) && IS_ENABLED(CONFIG_CPU_BIG_ENDIAN))
		__raw_writel(val, priv->base + offset);
	else
		writel_relaxed(val, priv->base + offset);
}