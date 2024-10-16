static int unimac_mdio_poll(void *wait_func_data)
{
	struct unimac_mdio_priv *priv = wait_func_data;
	unsigned int timeout = 1000;

	do {
		if (!unimac_mdio_busy(priv))
			return 0;

		usleep_range(1000, 2000);
	} while (--timeout);

	if (!timeout)
		return -ETIMEDOUT;

	return 0;
}