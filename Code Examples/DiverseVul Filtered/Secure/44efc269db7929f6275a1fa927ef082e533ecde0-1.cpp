static void adu_abort_transfers(struct adu_device *dev)
{
	unsigned long flags;

	if (dev->udev == NULL)
		return;

	/* shutdown transfer */

	/* XXX Anchor these instead */
	spin_lock_irqsave(&dev->buflock, flags);
	if (!dev->read_urb_finished) {
		spin_unlock_irqrestore(&dev->buflock, flags);
		usb_kill_urb(dev->interrupt_in_urb);
	} else
		spin_unlock_irqrestore(&dev->buflock, flags);

	spin_lock_irqsave(&dev->buflock, flags);
	if (!dev->out_urb_finished) {
		spin_unlock_irqrestore(&dev->buflock, flags);
		wait_event_timeout(dev->write_wait, dev->out_urb_finished,
			COMMAND_TIMEOUT);
		usb_kill_urb(dev->interrupt_out_urb);
	} else
		spin_unlock_irqrestore(&dev->buflock, flags);
}