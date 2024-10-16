static long dev_ioctl (struct file *fd, unsigned code, unsigned long value)
{
	struct dev_data		*dev = fd->private_data;
	struct usb_gadget	*gadget = dev->gadget;
	long ret = -ENOTTY;

	spin_lock_irq(&dev->lock);
	if (dev->state == STATE_DEV_OPENED ||
			dev->state == STATE_DEV_UNBOUND) {
		/* Not bound to a UDC */
	} else if (gadget->ops->ioctl) {
		++dev->udc_usage;
		spin_unlock_irq(&dev->lock);

		ret = gadget->ops->ioctl (gadget, code, value);

		spin_lock_irq(&dev->lock);
		--dev->udc_usage;
	}
	spin_unlock_irq(&dev->lock);

	return ret;
}