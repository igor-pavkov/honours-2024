static loff_t sisusb_lseek(struct file *file, loff_t offset, int orig)
{
	struct sisusb_usb_data *sisusb;
	loff_t ret;

	sisusb = file->private_data;
	if (!sisusb)
		return -ENODEV;

	mutex_lock(&sisusb->lock);

	/* Sanity check */
	if (!sisusb->present || !sisusb->ready || !sisusb->sisusb_dev) {
		mutex_unlock(&sisusb->lock);
		return -ENODEV;
	}

	ret = no_seek_end_llseek(file, offset, orig);

	mutex_unlock(&sisusb->lock);
	return ret;
}