static void usb_audio_make_shortname(struct usb_device *dev,
				     struct snd_usb_audio *chip,
				     const struct snd_usb_audio_quirk *quirk)
{
	struct snd_card *card = chip->card;

	if (quirk && quirk->product_name && *quirk->product_name) {
		strlcpy(card->shortname, quirk->product_name,
			sizeof(card->shortname));
		return;
	}

	/* retrieve the device string as shortname */
	if (!dev->descriptor.iProduct ||
	    usb_string(dev, dev->descriptor.iProduct,
		       card->shortname, sizeof(card->shortname)) <= 0) {
		/* no name available from anywhere, so use ID */
		sprintf(card->shortname, "USB Device %#04x:%#04x",
			USB_ID_VENDOR(chip->usb_id),
			USB_ID_PRODUCT(chip->usb_id));
	}

	strim(card->shortname);
}