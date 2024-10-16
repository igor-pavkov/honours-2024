static int proc_get_capabilities(struct usb_dev_state *ps, void __user *arg)
{
	__u32 caps;

	caps = USBDEVFS_CAP_ZERO_PACKET | USBDEVFS_CAP_NO_PACKET_SIZE_LIM |
			USBDEVFS_CAP_REAP_AFTER_DISCONNECT | USBDEVFS_CAP_MMAP |
			USBDEVFS_CAP_DROP_PRIVILEGES;
	if (!ps->dev->bus->no_stop_on_short)
		caps |= USBDEVFS_CAP_BULK_CONTINUATION;
	if (ps->dev->bus->sg_tablesize)
		caps |= USBDEVFS_CAP_BULK_SCATTER_GATHER;

	if (put_user(caps, (__u32 __user *)arg))
		return -EFAULT;

	return 0;
}