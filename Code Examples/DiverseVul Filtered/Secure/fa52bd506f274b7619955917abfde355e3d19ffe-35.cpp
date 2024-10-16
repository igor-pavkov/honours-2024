static int usbvision_v4l2_mmap(struct file *file, struct vm_area_struct *vma)
{
	struct usb_usbvision *usbvision = video_drvdata(file);
	int res;

	if (mutex_lock_interruptible(&usbvision->v4l2_lock))
		return -ERESTARTSYS;
	res = usbvision_mmap(file, vma);
	mutex_unlock(&usbvision->v4l2_lock);
	return res;
}