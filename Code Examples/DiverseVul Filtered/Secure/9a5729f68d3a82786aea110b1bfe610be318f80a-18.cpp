static int sisusb_release(struct inode *inode, struct file *file)
{
	struct sisusb_usb_data *sisusb;

	sisusb = file->private_data;
	if (!sisusb)
		return -ENODEV;

	mutex_lock(&sisusb->lock);

	if (sisusb->present) {
		/* Wait for all URBs to finish if device still present */
		if (!sisusb_wait_all_out_complete(sisusb))
			sisusb_kill_all_busy(sisusb);
	}

	sisusb->isopen = 0;
	file->private_data = NULL;

	mutex_unlock(&sisusb->lock);

	/* decrement the usage count on our device */
	kref_put(&sisusb->kref, sisusb_delete);

	return 0;
}