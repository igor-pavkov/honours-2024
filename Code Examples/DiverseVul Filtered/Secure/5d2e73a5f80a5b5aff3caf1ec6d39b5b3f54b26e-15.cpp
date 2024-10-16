static int send_control_msg(struct usb_device *udev, u8 request, u16 value,
			    u16 index, unsigned char *cp, u16 size)
{
	int status;

	unsigned char *transfer_buffer = kmalloc(size, GFP_KERNEL);
	if (!transfer_buffer)
		return -ENOMEM;

	memcpy(transfer_buffer, cp, size);

	status = usb_control_msg(udev,
				 usb_sndctrlpipe(udev, 0),
				 request,
				 USB_DIR_OUT | USB_TYPE_VENDOR |
				 USB_RECIP_DEVICE, value, index,
				 transfer_buffer, size, CTRL_TIMEOUT);

	kfree(transfer_buffer);
	return status;
}