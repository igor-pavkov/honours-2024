static void irda_getvalue_confirm(int result, __u16 obj_id,
				  struct ias_value *value, void *priv)
{
	struct irda_sock *self;

	self = (struct irda_sock *) priv;
	if (!self) {
		IRDA_WARNING("%s: lost myself!\n", __func__);
		return;
	}

	IRDA_DEBUG(2, "%s(%p)\n", __func__, self);

	/* We probably don't need to make any more queries */
	iriap_close(self->iriap);
	self->iriap = NULL;

	/* Check if request succeeded */
	if (result != IAS_SUCCESS) {
		IRDA_DEBUG(1, "%s(), IAS query failed! (%d)\n", __func__,
			   result);

		self->errno = result;	/* We really need it later */

		/* Wake up any processes waiting for result */
		wake_up_interruptible(&self->query_wait);

		return;
	}

	/* Pass the object to the caller (so the caller must delete it) */
	self->ias_result = value;
	self->errno = 0;

	/* Wake up any processes waiting for result */
	wake_up_interruptible(&self->query_wait);
}