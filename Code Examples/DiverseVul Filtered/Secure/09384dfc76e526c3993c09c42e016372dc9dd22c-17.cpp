static void irda_discovery_timeout(u_long priv)
{
	struct irda_sock *self;

	IRDA_DEBUG(2, "%s()\n", __func__);

	self = (struct irda_sock *) priv;
	BUG_ON(self == NULL);

	/* Nothing for the caller */
	self->cachelog = NULL;
	self->cachedaddr = 0;
	self->errno = -ETIME;

	/* Wake up process if its still waiting... */
	wake_up_interruptible(&self->query_wait);
}