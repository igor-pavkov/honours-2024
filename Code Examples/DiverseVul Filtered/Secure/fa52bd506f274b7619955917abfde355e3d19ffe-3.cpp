static ssize_t usbvision_v4l2_read(struct file *file, char __user *buf,
		      size_t count, loff_t *ppos)
{
	struct usb_usbvision *usbvision = video_drvdata(file);
	int res;

	if (mutex_lock_interruptible(&usbvision->v4l2_lock))
		return -ERESTARTSYS;
	res = usbvision_read(file, buf, count, ppos);
	mutex_unlock(&usbvision->v4l2_lock);
	return res;
}