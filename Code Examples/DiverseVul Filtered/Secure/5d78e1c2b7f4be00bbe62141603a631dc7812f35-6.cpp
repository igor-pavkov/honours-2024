unsigned char snd_usb_parse_datainterval(struct snd_usb_audio *chip,
					 struct usb_host_interface *alts)
{
	switch (snd_usb_get_speed(chip->dev)) {
	case USB_SPEED_HIGH:
	case USB_SPEED_WIRELESS:
	case USB_SPEED_SUPER:
	case USB_SPEED_SUPER_PLUS:
		if (get_endpoint(alts, 0)->bInterval >= 1 &&
		    get_endpoint(alts, 0)->bInterval <= 4)
			return get_endpoint(alts, 0)->bInterval - 1;
		break;
	default:
		break;
	}
	return 0;
}