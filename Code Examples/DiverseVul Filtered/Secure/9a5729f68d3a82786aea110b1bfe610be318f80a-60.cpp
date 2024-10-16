static ssize_t sisusb_write(struct file *file, const char __user *buffer,
		size_t count, loff_t *ppos)
{
	struct sisusb_usb_data *sisusb;
	int errno = 0;
	ssize_t bytes_written = 0;
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

		/* Write i/o ports
		 * Byte, word and long(32) can be written. As this
		 * emulates outX instructions, the data is expected
		 * in machine-endianness.
		 */
		switch (count) {
		case 1:
			if (get_user(buf8, (u8 __user *)buffer))
				errno = -EFAULT;
			else if (sisusb_write_memio_byte(sisusb,
					SISUSB_TYPE_IO, address, buf8))
				errno = -EIO;
			else
				bytes_written = 1;

			break;

		case 2:
			if (get_user(buf16, (u16 __user *)buffer))
				errno = -EFAULT;
			else if (sisusb_write_memio_word(sisusb,
					SISUSB_TYPE_IO, address, buf16))
				errno = -EIO;
			else
				bytes_written = 2;

			break;

		case 4:
			if (get_user(buf32, (u32 __user *)buffer))
				errno = -EFAULT;
			else if (sisusb_write_memio_long(sisusb,
					SISUSB_TYPE_IO, address, buf32))
				errno = -EIO;
			else
				bytes_written = 4;

			break;

		default:
			errno = -EIO;
		}

	} else if ((*ppos) >= SISUSB_PCI_PSEUDO_MEMBASE &&
			(*ppos) <  SISUSB_PCI_PSEUDO_MEMBASE +
			sisusb->vramsize) {

		address = (*ppos) - SISUSB_PCI_PSEUDO_MEMBASE +
				SISUSB_PCI_MEMBASE;

		/* Write video ram.
		 * Buffer is copied 1:1, therefore, on big-endian
		 * machines, the data must be swapped by userland
		 * in advance (if applicable; no swapping in 8bpp
		 * mode or if YUV data is being transferred).
		 */
		errno = sisusb_write_mem_bulk(sisusb, address, NULL,
				count, buffer, 0, &bytes_written);

		if (bytes_written)
			errno = bytes_written;

	} else  if ((*ppos) >= SISUSB_PCI_PSEUDO_MMIOBASE &&
			(*ppos) <  SISUSB_PCI_PSEUDO_MMIOBASE +
			SISUSB_PCI_MMIOSIZE) {

		address = (*ppos) - SISUSB_PCI_PSEUDO_MMIOBASE +
				SISUSB_PCI_MMIOBASE;

		/* Write MMIO.
		 * Buffer is copied 1:1, therefore, on big-endian
		 * machines, the data must be swapped by userland
		 * in advance.
		 */
		errno = sisusb_write_mem_bulk(sisusb, address, NULL,
				count, buffer, 0, &bytes_written);

		if (bytes_written)
			errno = bytes_written;

	} else  if ((*ppos) >= SISUSB_PCI_PSEUDO_PCIBASE &&
				(*ppos) <= SISUSB_PCI_PSEUDO_PCIBASE +
				SISUSB_PCI_PCONFSIZE) {

		if (count != 4) {
			mutex_unlock(&sisusb->lock);
			return -EINVAL;
		}

		address = (*ppos) - SISUSB_PCI_PSEUDO_PCIBASE;

		/* Write PCI config register.
		 * Given value expected in machine endianness.
		 */
		if (get_user(buf32, (u32 __user *)buffer))
			errno = -EFAULT;
		else if (sisusb_write_pci_config(sisusb, address, buf32))
			errno = -EIO;
		else
			bytes_written = 4;


	} else {

		/* Error */
		errno = -EBADFD;

	}

	(*ppos) += bytes_written;

	mutex_unlock(&sisusb->lock);

	return errno ? errno : bytes_written;
}