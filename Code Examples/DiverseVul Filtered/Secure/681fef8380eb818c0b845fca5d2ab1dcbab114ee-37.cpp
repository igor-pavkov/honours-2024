static int proc_submiturb_compat(struct usb_dev_state *ps, void __user *arg)
{
	struct usbdevfs_urb uurb;

	if (get_urb32(&uurb, (struct usbdevfs_urb32 __user *)arg))
		return -EFAULT;

	return proc_do_submiturb(ps, &uurb,
			((struct usbdevfs_urb32 __user *)arg)->iso_frame_desc,
			arg);
}