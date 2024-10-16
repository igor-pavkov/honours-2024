static ssize_t sisusb_read(struct file *file, char __user *buffer,
		size_t count, loff_t *ppos)
{
	struct sisusb_usb_data *sisusb;
	ssize_t bytes_read = 0;
	int errno = 0;
	u8 buf8;
	u16 buf16;
	u32 buf32, address;

	sisusb = file->private_data;
	if (!sisusb)
		return -ENODEV;

	mutex_lock(&sisusb->lock);

	/* Sanity check */
	if (!sisusb->present || !sisusb->ready || !sisusb->sisusb_dev) {
		mutex_unlock(&sisusb->lock);
		return -ENODEV;
	}

	if ((*ppos) >= SISUSB_PCI_PSEUDO_IOPORTBASE &&
			(*ppos) <  SISUSB_PCI_PSEUDO_IOPORTBASE + 128) {

		address = (*ppos) - SISUSB_PCI_PSEUDO_IOPORTBASE +
				SISUSB_PCI_IOPORTBASE;

		/* Read i/o ports
		 * Byte, word and long(32) can be read. As this
		 * emulates inX instructions, the data returned is
		 * in machine-endianness.
		 */
		switch (count) {
		case 1:
			if (sisusb_read_memio_byte(sisusb, SISUSB_TYPE_IO,
					address, &buf8))
				errno = -EIO;
			else if (put_user(buf8, (u8 __user *)buffer))
				errno = -EFAULT;
			else
				bytes_read = 1;

			break;

		case 2:
			if (sisusb_read_memio_word(sisusb, SISUSB_TYPE_IO,
					address, &buf16))
				errno = -EIO;
			else if (put_user(buf16, (u16 __user *)buffer))
				errno = -EFAULT;
			else
				bytes_read = 2;

			break;

		case 4:
			if (sisusb_read_memio_long(sisusb, SISUSB_TYPE_IO,
					address, &buf32))
				errno = -EIO;
			else if (put_user(buf32, (u32 __user *)buffer))
				errno = -EFAULT;
			else
				bytes_read = 4;

			break;

		default:
			errno = -EIO;

		}

	} else if ((*ppos) >= SISUSB_PCI_PSEUDO_MEMBASE && (*ppos) <
			SISUSB_PCI_PSEUDO_MEMBASE + sisusb->vramsize) {

		address = (*ppos) - SISUSB_PCI_PSEUDO_MEMBASE +
				SISUSB_PCI_MEMBASE;

		/* Read video ram
		 * Remember: Data delivered is never endian-corrected
		 */
		errno = sisusb_read_mem_bulk(sisusb, address,
				NULL, count, buffer, &bytes_read);

		if (bytes_read)
			errno = bytes_read;

	} else  if ((*ppos) >= SISUSB_PCI_PSEUDO_MMIOBASE &&
				(*ppos) <  SISUSB_PCI_PSEUDO_MMIOBASE +
				SISUSB_PCI_MMIOSIZE) {

		address = (*ppos) - SISUSB_PCI_PSEUDO_MMIOBASE +
				SISUSB_PCI_MMIOBASE;

		/* Read MMIO
		 * Remember: Data delivered is never endian-corrected
		 */
		errno = sisusb_read_mem_bulk(sisusb, address,
				NULL, count, buffer, &bytes_read);

		if (bytes_read)
			errno = bytes_read;

	} else  if ((*ppos) >= SISUSB_PCI_PSEUDO_PCIBASE &&
			(*ppos) <= SISUSB_PCI_PSEUDO_PCIBASE + 0x5c) {

		if (count != 4) {
			mutex_unlock(&sisusb->lock);
			return -EINVAL;
		}

		address = (*ppos) - SISUSB_PCI_PSEUDO_PCIBASE;

		/* Read PCI config register
		 * Return value delivered in machine endianness.
		 */
		if (sisusb_read_pci_config(sisusb, address, &buf32))
			errno = -EIO;
		else if (put_user(buf32, (u32 __user *)buffer))
			errno = -EFAULT;
		else
			bytes_read = 4;

	} else {

		errno = -EBADFD;

	}

	(*ppos) += bytes_read;

	mutex_unlock(&sisusb->lock);

	return errno ? errno : bytes_read;
}