void stub_complete(struct urb *urb)
{
	struct stub_priv *priv = (struct stub_priv *) urb->context;
	struct stub_device *sdev = priv->sdev;
	unsigned long flags;

	usbip_dbg_stub_tx("complete! status %d\n", urb->status);

	switch (urb->status) {
	case 0:
		/* OK */
		break;
	case -ENOENT:
		dev_info(&urb->dev->dev,
			 "stopped by a call to usb_kill_urb() because of cleaning up a virtual connection\n");
		return;
	case -ECONNRESET:
		dev_info(&urb->dev->dev,
			 "unlinked by a call to usb_unlink_urb()\n");
		break;
	case -EPIPE:
		dev_info(&urb->dev->dev, "endpoint %d is stalled\n",
			 usb_pipeendpoint(urb->pipe));
		break;
	case -ESHUTDOWN:
		dev_info(&urb->dev->dev, "device removed?\n");
		break;
	default:
		dev_info(&urb->dev->dev,
			 "urb completion with non-zero status %d\n",
			 urb->status);
		break;
	}

	/* link a urb to the queue of tx. */
	spin_lock_irqsave(&sdev->priv_lock, flags);
	if (sdev->ud.tcp_socket == NULL) {
		usbip_dbg_stub_tx("ignore urb for closed connection %p", urb);
		/* It will be freed in stub_device_cleanup_urbs(). */
	} else if (priv->unlinking) {
		stub_enqueue_ret_unlink(sdev, priv->seqnum, urb->status);
		stub_free_priv_and_urb(priv);
	} else {
		list_move_tail(&priv->list, &sdev->priv_tx);
	}
	spin_unlock_irqrestore(&sdev->priv_lock, flags);

	/* wake up tx_thread */
	wake_up(&sdev->tx_waitq);
}