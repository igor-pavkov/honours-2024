static void technisat_usb2_frontend_reset(struct usb_device *udev)
{
	usb_control_msg(udev, usb_sndctrlpipe(udev, 0),
			SET_FRONT_END_RESET_VENDOR_REQUEST,
			USB_TYPE_VENDOR | USB_DIR_OUT,
			10, 0,
			NULL, 0, 500);
}