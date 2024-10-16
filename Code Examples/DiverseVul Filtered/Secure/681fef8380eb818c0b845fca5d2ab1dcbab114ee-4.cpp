static int checkintf(struct usb_dev_state *ps, unsigned int ifnum)
{
	if (ps->dev->state != USB_STATE_CONFIGURED)
		return -EHOSTUNREACH;
	if (ifnum >= 8*sizeof(ps->ifclaimed))
		return -EINVAL;
	if (test_bit(ifnum, &ps->ifclaimed))
		return 0;
	/* if not yet claimed, claim it for the driver */
	dev_warn(&ps->dev->dev, "usbfs: process %d (%s) did not claim "
		 "interface %u before use\n", task_pid_nr(current),
		 current->comm, ifnum);
	return claimintf(ps, ifnum);
}