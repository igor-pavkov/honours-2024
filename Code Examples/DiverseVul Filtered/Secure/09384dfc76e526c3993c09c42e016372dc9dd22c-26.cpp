static void irda_selective_discovery_indication(discinfo_t *discovery,
						DISCOVERY_MODE mode,
						void *priv)
{
	struct irda_sock *self;

	IRDA_DEBUG(2, "%s()\n", __func__);

	self = (struct irda_sock *) priv;
	if (!self) {
		IRDA_WARNING("%s: lost myself!\n", __func__);
		return;
	}

	/* Pass parameter to the caller */
	self->cachedaddr = discovery->daddr;

	/* Wake up process if its waiting for device to be discovered */
	wake_up_interruptible(&self->query_wait);
}