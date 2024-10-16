static int sisusb_init_gfxdevice(struct sisusb_usb_data *sisusb, int initscreen)
{
	int ret = 0, test = 0;
	u32 tmp32;

	if (sisusb->devinit == 1) {
		/* Read PCI BARs and see if they have been set up */
		ret |= sisusb_read_pci_config(sisusb, 0x10, &tmp32);
		if (ret)
			return ret;

		if ((tmp32 & 0xfffffff0) == SISUSB_PCI_MEMBASE)
			test++;

		ret |= sisusb_read_pci_config(sisusb, 0x14, &tmp32);
		if (ret)
			return ret;

		if ((tmp32 & 0xfffffff0) == SISUSB_PCI_MMIOBASE)
			test++;

		ret |= sisusb_read_pci_config(sisusb, 0x18, &tmp32);
		if (ret)
			return ret;

		if ((tmp32 & 0xfffffff0) == SISUSB_PCI_IOPORTBASE)
			test++;
	}

	/* No? So reset the device */
	if ((sisusb->devinit == 0) || (test != 3)) {

		ret |= sisusb_do_init_gfxdevice(sisusb);

		if (ret == 0)
			sisusb->devinit = 1;

	}

	if (sisusb->devinit) {
		/* Initialize the graphics core */
		if (sisusb_init_gfxcore(sisusb) == 0) {
			sisusb->gfxinit = 1;
			sisusb_get_ramconfig(sisusb);
			ret |= sisusb_set_default_mode(sisusb, 1);
			ret |= sisusb_setup_screen(sisusb, 1, initscreen);
		}
	}

	return ret;
}