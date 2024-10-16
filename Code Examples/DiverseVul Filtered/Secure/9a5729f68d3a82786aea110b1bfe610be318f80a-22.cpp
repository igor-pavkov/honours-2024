static int sisusb_recv_bulk_msg(struct sisusb_usb_data *sisusb, int ep, int len,
		void *kernbuffer, char __user *userbuffer, ssize_t *bytes_read,
		unsigned int tflags)
{
	int result = 0, retry, count = len;
	int bufsize, thispass, transferred_len;
	unsigned int pipe;
	char *buffer;

	(*bytes_read) = 0;

	/* Sanity check */
	if (!sisusb || !sisusb->present || !sisusb->sisusb_dev)
		return -ENODEV;

	pipe = usb_rcvbulkpipe(sisusb->sisusb_dev, ep);
	buffer = sisusb->ibuf;
	bufsize = sisusb->ibufsize;

	retry = 5;

#ifdef SISUSB_DONTSYNC
	if (!(sisusb_wait_all_out_complete(sisusb)))
		return -EIO;
#endif

	while (count > 0) {

		if (!sisusb->sisusb_dev)
			return -ENODEV;

		thispass = (bufsize < count) ? bufsize : count;

		result = sisusb_bulkin_msg(sisusb, pipe, buffer, thispass,
				&transferred_len, 5 * HZ, tflags);

		if (transferred_len)
			thispass = transferred_len;

		else if (result == -ETIMEDOUT) {

			if (!retry--)
				return -ETIME;

			continue;

		} else
			return -EIO;


		if (thispass) {

			(*bytes_read) += thispass;
			count         -= thispass;

			if (userbuffer) {

				if (copy_to_user(userbuffer, buffer, thispass))
					return -EFAULT;

				userbuffer += thispass;

			} else {

				memcpy(kernbuffer, buffer, thispass);
				kernbuffer += thispass;

			}

		}

	}

	return ((*bytes_read) == len) ? 0 : -EIO;
}