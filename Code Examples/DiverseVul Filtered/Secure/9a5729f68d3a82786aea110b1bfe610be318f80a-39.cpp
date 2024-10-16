static int sisusb_handle_command(struct sisusb_usb_data *sisusb,
		struct sisusb_command *y, unsigned long arg)
{
	int	retval, port, length;
	u32	address;

	/* All our commands require the device
	 * to be initialized.
	 */
	if (!sisusb->devinit)
		return -ENODEV;

	port = y->data3 -
		SISUSB_PCI_PSEUDO_IOPORTBASE +
		SISUSB_PCI_IOPORTBASE;

	switch (y->operation) {
	case SUCMD_GET:
		retval = sisusb_getidxreg(sisusb, port, y->data0, &y->data1);
		if (!retval) {
			if (copy_to_user((void __user *)arg, y, sizeof(*y)))
				retval = -EFAULT;
		}
		break;

	case SUCMD_SET:
		retval = sisusb_setidxreg(sisusb, port, y->data0, y->data1);
		break;

	case SUCMD_SETOR:
		retval = sisusb_setidxregor(sisusb, port, y->data0, y->data1);
		break;

	case SUCMD_SETAND:
		retval = sisusb_setidxregand(sisusb, port, y->data0, y->data1);
		break;

	case SUCMD_SETANDOR:
		retval = sisusb_setidxregandor(sisusb, port, y->data0,
				y->data1, y->data2);
		break;

	case SUCMD_SETMASK:
		retval = sisusb_setidxregmask(sisusb, port, y->data0,
				y->data1, y->data2);
		break;

	case SUCMD_CLRSCR:
		/* Gfx core must be initialized */
		if (!sisusb->gfxinit)
			return -ENODEV;

		length = (y->data0 << 16) | (y->data1 << 8) | y->data2;
		address = y->data3 - SISUSB_PCI_PSEUDO_MEMBASE +
				SISUSB_PCI_MEMBASE;
		retval = sisusb_clear_vram(sisusb, address, length);
		break;

	case SUCMD_HANDLETEXTMODE:
		retval = 0;
#ifdef CONFIG_USB_SISUSBVGA_CON
		/* Gfx core must be initialized, SiS_Pr must exist */
		if (!sisusb->gfxinit || !sisusb->SiS_Pr)
			return -ENODEV;

		switch (y->data0) {
		case 0:
			retval = sisusb_reset_text_mode(sisusb, 0);
			break;
		case 1:
			sisusb->textmodedestroyed = 1;
			break;
		}
#endif
		break;

#ifdef CONFIG_USB_SISUSBVGA_CON
	case SUCMD_SETMODE:
		/* Gfx core must be initialized, SiS_Pr must exist */
		if (!sisusb->gfxinit || !sisusb->SiS_Pr)
			return -ENODEV;

		retval = 0;

		sisusb->SiS_Pr->IOAddress = SISUSB_PCI_IOPORTBASE + 0x30;
		sisusb->SiS_Pr->sisusb = (void *)sisusb;

		if (SiSUSBSetMode(sisusb->SiS_Pr, y->data3))
			retval = -EINVAL;

		break;

	case SUCMD_SETVESAMODE:
		/* Gfx core must be initialized, SiS_Pr must exist */
		if (!sisusb->gfxinit || !sisusb->SiS_Pr)
			return -ENODEV;

		retval = 0;

		sisusb->SiS_Pr->IOAddress = SISUSB_PCI_IOPORTBASE + 0x30;
		sisusb->SiS_Pr->sisusb = (void *)sisusb;

		if (SiSUSBSetVESAMode(sisusb->SiS_Pr, y->data3))
			retval = -EINVAL;

		break;
#endif

	default:
		retval = -EINVAL;
	}

	if (retval > 0)
		retval = -EIO;

	return retval;
}