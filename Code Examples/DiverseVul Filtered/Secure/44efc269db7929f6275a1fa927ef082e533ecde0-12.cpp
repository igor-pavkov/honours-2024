static void adu_interrupt_out_callback(struct urb *urb)
{
	struct adu_device *dev = urb->context;
	int status = urb->status;
	unsigned long flags;

	adu_debug_data(&dev->udev->dev, __func__,
		       urb->actual_length, urb->transfer_buffer);

	if (status != 0) {
		if ((status != -ENOENT) &&
		    (status != -ECONNRESET)) {
			dev_dbg(&dev->udev->dev,
				"%s :nonzero status received: %d\n", __func__,
				status);
		}
		return;
	}

	spin_lock_irqsave(&dev->buflock, flags);
	dev->out_urb_finished = 1;
	wake_up(&dev->write_wait);
	spin_unlock_irqrestore(&dev->buflock, flags);
}