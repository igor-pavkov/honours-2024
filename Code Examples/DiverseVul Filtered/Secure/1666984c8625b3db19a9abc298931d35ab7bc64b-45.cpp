int usbnet_resume (struct usb_interface *intf)
{
	struct usbnet		*dev = usb_get_intfdata(intf);
	struct sk_buff          *skb;
	struct urb              *res;
	int                     retval;

	if (!--dev->suspend_count) {
		/* resume interrupt URB if it was previously submitted */
		__usbnet_status_start_force(dev, GFP_NOIO);

		spin_lock_irq(&dev->txq.lock);
		while ((res = usb_get_from_anchor(&dev->deferred))) {

			skb = (struct sk_buff *)res->context;
			retval = usb_submit_urb(res, GFP_ATOMIC);
			if (retval < 0) {
				dev_kfree_skb_any(skb);
				kfree(res->sg);
				usb_free_urb(res);
				usb_autopm_put_interface_async(dev->intf);
			} else {
				dev->net->trans_start = jiffies;
				__skb_queue_tail(&dev->txq, skb);
			}
		}

		smp_mb();
		clear_bit(EVENT_DEV_ASLEEP, &dev->flags);
		spin_unlock_irq(&dev->txq.lock);

		if (test_bit(EVENT_DEV_OPEN, &dev->flags)) {
			/* handle remote wakeup ASAP
			 * we cannot race against stop
			 */
			if (netif_device_present(dev->net) &&
				!timer_pending(&dev->delay) &&
				!test_bit(EVENT_RX_HALT, &dev->flags))
					rx_alloc_submit(dev, GFP_NOIO);

			if (!(dev->txq.qlen >= TX_QLEN(dev)))
				netif_tx_wake_all_queues(dev->net);
			tasklet_schedule (&dev->bh);
		}
	}

	if (test_and_clear_bit(EVENT_DEVICE_REPORT_IDLE, &dev->flags))
		usb_autopm_get_interface_no_resume(intf);

	return 0;
}