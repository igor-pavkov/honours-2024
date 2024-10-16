static void adu_interrupt_in_callback(struct urb *urb)
{
	struct adu_device *dev = urb->context;
	int status = urb->status;
	unsigned long flags;

	adu_debug_data(&dev->udev->dev, __func__,
		       urb->actual_length, urb->transfer_buffer);

	spin_lock_irqsave(&dev->buflock, flags);

	if (status != 0) {
		if ((status != -ENOENT) && (status != -ECONNRESET) &&
			(status != -ESHUTDOWN)) {
			dev_dbg(&dev->udev->dev,
				"%s : nonzero status received: %d\n",
				__func__, status);
		}
		goto exit;
	}

	if (urb->actual_length > 0 && dev->interrupt_in_buffer[0] != 0x00) {
		if (dev->read_buffer_length <
		    (4 * usb_endpoint_maxp(dev->interrupt_in_endpoint)) -
		     (urb->actual_length)) {
			memcpy (dev->read_buffer_primary +
				dev->read_buffer_length,
				dev->interrupt_in_buffer, urb->actual_length);

			dev->read_buffer_length += urb->actual_length;
			dev_dbg(&dev->udev->dev,"%s reading  %d\n", __func__,
				urb->actual_length);
		} else {
			dev_dbg(&dev->udev->dev,"%s : read_buffer overflow\n",
				__func__);
		}
	}

exit:
	dev->read_urb_finished = 1;
	spin_unlock_irqrestore(&dev->buflock, flags);
	/* always wake up so we recover from errors */
	wake_up_interruptible(&dev->read_wait);
}