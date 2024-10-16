int usbnet_write_cmd_async(struct usbnet *dev, u8 cmd, u8 reqtype,
			   u16 value, u16 index, const void *data, u16 size)
{
	struct usb_ctrlrequest *req = NULL;
	struct urb *urb;
	int err = -ENOMEM;
	void *buf = NULL;

	netdev_dbg(dev->net, "usbnet_write_cmd cmd=0x%02x reqtype=%02x"
		   " value=0x%04x index=0x%04x size=%d\n",
		   cmd, reqtype, value, index, size);

	urb = usb_alloc_urb(0, GFP_ATOMIC);
	if (!urb) {
		netdev_err(dev->net, "Error allocating URB in"
			   " %s!\n", __func__);
		goto fail;
	}

	if (data) {
		buf = kmemdup(data, size, GFP_ATOMIC);
		if (!buf) {
			netdev_err(dev->net, "Error allocating buffer"
				   " in %s!\n", __func__);
			goto fail_free;
		}
	}

	req = kmalloc(sizeof(struct usb_ctrlrequest), GFP_ATOMIC);
	if (!req)
		goto fail_free_buf;

	req->bRequestType = reqtype;
	req->bRequest = cmd;
	req->wValue = cpu_to_le16(value);
	req->wIndex = cpu_to_le16(index);
	req->wLength = cpu_to_le16(size);

	usb_fill_control_urb(urb, dev->udev,
			     usb_sndctrlpipe(dev->udev, 0),
			     (void *)req, buf, size,
			     usbnet_async_cmd_cb, req);
	urb->transfer_flags |= URB_FREE_BUFFER;

	err = usb_submit_urb(urb, GFP_ATOMIC);
	if (err < 0) {
		netdev_err(dev->net, "Error submitting the control"
			   " message: status=%d\n", err);
		goto fail_free;
	}
	return 0;

fail_free_buf:
	kfree(buf);
fail_free:
	kfree(req);
	usb_free_urb(urb);
fail:
	return err;

}