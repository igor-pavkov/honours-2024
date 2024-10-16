static int palm_os_4_probe(struct usb_serial *serial,
						const struct usb_device_id *id)
{
	struct device *dev = &serial->dev->dev;
	struct palm_ext_connection_info *connection_info;
	unsigned char *transfer_buffer;
	int retval;

	transfer_buffer =  kmalloc(sizeof(*connection_info), GFP_KERNEL);
	if (!transfer_buffer)
		return -ENOMEM;

	retval = usb_control_msg(serial->dev,
				  usb_rcvctrlpipe(serial->dev, 0),
				  PALM_GET_EXT_CONNECTION_INFORMATION,
				  0xc2, 0x0000, 0x0000, transfer_buffer,
				  sizeof(*connection_info), 300);
	if (retval < 0)
		dev_err(dev, "%s - error %d getting connection info\n",
			__func__, retval);
	else
		usb_serial_debug_data(dev, __func__, retval, transfer_buffer);

	kfree(transfer_buffer);
	return 0;
}