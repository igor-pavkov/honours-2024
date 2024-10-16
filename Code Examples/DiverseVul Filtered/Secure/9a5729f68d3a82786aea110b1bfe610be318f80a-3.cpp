static int sisusb_send_bulk_msg(struct sisusb_usb_data *sisusb, int ep, int len,
		char *kernbuffer, const char __user *userbuffer, int index,
		ssize_t *bytes_written, unsigned int tflags, int async)
{
	int result = 0, retry, count = len;
	int passsize, thispass, transferred_len = 0;
	int fromuser = (userbuffer != NULL) ? 1 : 0;
	int fromkern = (kernbuffer != NULL) ? 1 : 0;
	unsigned int pipe;
	char *buffer;

	(*bytes_written) = 0;

	/* Sanity check */
	if (!sisusb || !sisusb->present || !sisusb->sisusb_dev)
		return -ENODEV;

	/* If we copy data from kernel or userspace, force the
	 * allocation of a buffer/urb. If we have the data in
	 * the transfer buffer[index] already, reuse the buffer/URB
	 * if the length is > buffer size. (So, transmitting
	 * large data amounts directly from the transfer buffer
	 * treats the buffer as a ring buffer. However, we need
	 * to sync in this case.)
	 */
	if (fromuser || fromkern)
		index = -1;
	else if (len > sisusb->obufsize)
		async = 0;

	pipe = usb_sndbulkpipe(sisusb->sisusb_dev, ep);

	do {
		passsize = thispass = (sisusb->obufsize < count) ?
				sisusb->obufsize : count;

		if (index < 0)
			index = sisusb_get_free_outbuf(sisusb);

		if (index < 0)
			return -EIO;

		buffer = sisusb->obuf[index];

		if (fromuser) {

			if (copy_from_user(buffer, userbuffer, passsize))
				return -EFAULT;

			userbuffer += passsize;

		} else if (fromkern) {

			memcpy(buffer, kernbuffer, passsize);
			kernbuffer += passsize;

		}

		retry = 5;
		while (thispass) {

			if (!sisusb->sisusb_dev)
				return -ENODEV;

			result = sisusb_bulkout_msg(sisusb, index, pipe,
					buffer, thispass, &transferred_len,
					async ? 0 : 5 * HZ, tflags);

			if (result == -ETIMEDOUT) {

				/* Will not happen if async */
				if (!retry--)
					return -ETIME;

				continue;
			}

			if ((result == 0) && !async && transferred_len) {

				thispass -= transferred_len;
				buffer += transferred_len;

			} else
				break;
		}

		if (result)
			return result;

		(*bytes_written) += passsize;
		count            -= passsize;

		/* Force new allocation in next iteration */
		if (fromuser || fromkern)
			index = -1;

	} while (count > 0);

	if (async) {
#ifdef SISUSB_DONTSYNC
		(*bytes_written) = len;
		/* Some URBs/buffers might be busy */
#else
		sisusb_wait_all_out_complete(sisusb);
		(*bytes_written) = transferred_len;
		/* All URBs and all buffers are available */
#endif
	}

	return ((*bytes_written) == len) ? 0 : -EIO;
}