static int ep_aio_cancel(struct kiocb *iocb)
{
	struct kiocb_priv	*priv = iocb->private;
	struct ep_data		*epdata;
	int			value;

	local_irq_disable();
	epdata = priv->epdata;
	// spin_lock(&epdata->dev->lock);
	if (likely(epdata && epdata->ep && priv->req))
		value = usb_ep_dequeue (epdata->ep, priv->req);
	else
		value = -EINVAL;
	// spin_unlock(&epdata->dev->lock);
	local_irq_enable();

	return value;
}