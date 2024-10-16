static int smsusb_sendrequest(void *context, void *buffer, size_t size)
{
	struct smsusb_device_t *dev = (struct smsusb_device_t *) context;
	struct sms_msg_hdr *phdr;
	int dummy, ret;

	if (dev->state != SMSUSB_ACTIVE) {
		pr_debug("Device not active yet\n");
		return -ENOENT;
	}

	phdr = kmemdup(buffer, size, GFP_KERNEL);
	if (!phdr)
		return -ENOMEM;

	pr_debug("sending %s(%d) size: %d\n",
		  smscore_translate_msg(phdr->msg_type), phdr->msg_type,
		  phdr->msg_length);

	smsendian_handle_tx_message((struct sms_msg_data *) phdr);
	smsendian_handle_message_header((struct sms_msg_hdr *)phdr);
	ret = usb_bulk_msg(dev->udev, usb_sndbulkpipe(dev->udev, 2),
			    phdr, size, &dummy, 1000);

	kfree(phdr);
	return ret;
}