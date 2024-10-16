static int is_valid_config(struct usb_config_descriptor *config,
		unsigned int total)
{
	return config->bDescriptorType == USB_DT_CONFIG
		&& config->bLength == USB_DT_CONFIG_SIZE
		&& total >= USB_DT_CONFIG_SIZE
		&& config->bConfigurationValue != 0
		&& (config->bmAttributes & USB_CONFIG_ATT_ONE) != 0
		&& (config->bmAttributes & USB_CONFIG_ATT_WAKEUP) == 0;
	/* FIXME if gadget->is_otg, _must_ include an otg descriptor */
	/* FIXME check lengths: walk to end */
}