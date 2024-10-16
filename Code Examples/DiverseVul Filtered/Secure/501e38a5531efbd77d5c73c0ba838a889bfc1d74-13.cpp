static ssize_t ep_aio(struct kiocb *iocb,
		      struct kiocb_priv *priv,
		      struct ep_data *epdata,
		      char *buf,
		      size_t len)
{
	struct usb_request *req;
	ssize_t value;

	iocb->private = priv;
	priv->iocb = iocb;

	kiocb_set_cancel_fn(iocb, ep_aio_cancel);
	get_ep(epdata);
	priv->epdata = epdata;
	priv->actual = 0;
	priv->mm = current->mm; /* mm teardown waits for iocbs in exit_aio() */

	/* each kiocb is coupled to one usb_request, but we can't
	 * allocate or submit those if the host disconnected.
	 */
	spin_lock_irq(&epdata->dev->lock);
	value = -ENODEV;
	if (unlikely(epdata->ep == NULL))
		goto fail;

	req = usb_ep_alloc_request(epdata->ep, GFP_ATOMIC);
	value = -ENOMEM;
	if (unlikely(!req))
		goto fail;

	priv->req = req;
	req->buf = buf;
	req->length = len;
	req->complete = ep_aio_complete;
	req->context = iocb;
	value = usb_ep_queue(epdata->ep, req, GFP_ATOMIC);
	if (unlikely(0 != value)) {
		usb_ep_free_request(epdata->ep, req);
		goto fail;
	}
	spin_unlock_irq(&epdata->dev->lock);
	return -EIOCBQUEUED;

fail:
	spin_unlock_irq(&epdata->dev->lock);
	kfree(priv->to_free);
	kfree(priv);
	put_ep(epdata);
	return value;
}