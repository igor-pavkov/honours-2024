static long sisusb_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct sisusb_usb_data *sisusb;
	struct sisusb_info x;
	struct sisusb_command y;
	long retval = 0;
	u32 __user *argp = (u32 __user *)arg;

	sisusb = file->private_data;
	if (!sisusb)
		return -ENODEV;

	mutex_lock(&sisusb->lock);

	/* Sanity check */
	if (!sisusb->present || !sisusb->ready || !sisusb->sisusb_dev) {
		retval = -ENODEV;
		goto err_out;
	}

	switch (cmd) {
	case SISUSB_GET_CONFIG_SIZE:

		if (put_user(sizeof(x), argp))
			retval = -EFAULT;

		break;

	case SISUSB_GET_CONFIG:

		x.sisusb_id = SISUSB_ID;
		x.sisusb_version = SISUSB_VERSION;
		x.sisusb_revision = SISUSB_REVISION;
		x.sisusb_patchlevel = SISUSB_PATCHLEVEL;
		x.sisusb_gfxinit = sisusb->gfxinit;
		x.sisusb_vrambase = SISUSB_PCI_PSEUDO_MEMBASE;
		x.sisusb_mmiobase = SISUSB_PCI_PSEUDO_MMIOBASE;
		x.sisusb_iobase = SISUSB_PCI_PSEUDO_IOPORTBASE;
		x.sisusb_pcibase = SISUSB_PCI_PSEUDO_PCIBASE;
		x.sisusb_vramsize = sisusb->vramsize;
		x.sisusb_minor = sisusb->minor;
		x.sisusb_fbdevactive = 0;
#ifdef CONFIG_USB_SISUSBVGA_CON
		x.sisusb_conactive  = sisusb->haveconsole ? 1 : 0;
#else
		x.sisusb_conactive  = 0;
#endif
		memset(x.sisusb_reserved, 0, sizeof(x.sisusb_reserved));

		if (copy_to_user((void __user *)arg, &x, sizeof(x)))
			retval = -EFAULT;

		break;

	case SISUSB_COMMAND:

		if (copy_from_user(&y, (void __user *)arg, sizeof(y)))
			retval = -EFAULT;
		else
			retval = sisusb_handle_command(sisusb, &y, arg);

		break;

	default:
		retval = -ENOTTY;
		break;
	}

err_out:
	mutex_unlock(&sisusb->lock);
	return retval;
}