static void usb_audio_make_longname(struct usb_device *dev,
				    struct snd_usb_audio *chip,
				    const struct snd_usb_audio_quirk *quirk)
{
	struct snd_card *card = chip->card;
	int len;

	/* shortcut - if any pre-defined string is given, use it */
	if (quirk && quirk->profile_name && *quirk->profile_name) {
		strlcpy(card->longname, quirk->profile_name,
			sizeof(card->longname));
		return;
	}

	if (quirk && quirk->vendor_name && *quirk->vendor_name) {
		len = strlcpy(card->longname, quirk->vendor_name, sizeof(card->longname));
	} else {
		/* retrieve the vendor and device strings as longname */
		if (dev->descriptor.iManufacturer)
			len = usb_string(dev, dev->descriptor.iManufacturer,
					 card->longname, sizeof(card->longname));
		else
			len = 0;
		/* we don't really care if there isn't any vendor string */
	}
	if (len > 0) {
		strim(card->longname);
		if (*card->longname)
			strlcat(card->longname, " ", sizeof(card->longname));
	}

	strlcat(card->longname, card->shortname, sizeof(card->longname));

	len = strlcat(card->longname, " at ", sizeof(card->longname));

	if (len < sizeof(card->longname))
		usb_make_path(dev, card->longname + len, sizeof(card->longname) - len);

	switch (snd_usb_get_speed(dev)) {
	case USB_SPEED_LOW:
		strlcat(card->longname, ", low speed", sizeof(card->longname));
		break;
	case USB_SPEED_FULL:
		strlcat(card->longname, ", full speed", sizeof(card->longname));
		break;
	case USB_SPEED_HIGH:
		strlcat(card->longname, ", high speed", sizeof(card->longname));
		break;
	case USB_SPEED_SUPER:
		strlcat(card->longname, ", super speed", sizeof(card->longname));
		break;
	case USB_SPEED_SUPER_PLUS:
		strlcat(card->longname, ", super speed plus", sizeof(card->longname));
		break;
	default:
		break;
	}
}