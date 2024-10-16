bool hid_ignore(struct hid_device *hdev)
{
	if (hdev->quirks & HID_QUIRK_NO_IGNORE)
		return false;
	if (hdev->quirks & HID_QUIRK_IGNORE)
		return true;

	switch (hdev->vendor) {
	case USB_VENDOR_ID_CODEMERCS:
		/* ignore all Code Mercenaries IOWarrior devices */
		if (hdev->product >= USB_DEVICE_ID_CODEMERCS_IOW_FIRST &&
				hdev->product <= USB_DEVICE_ID_CODEMERCS_IOW_LAST)
			return true;
		break;
	case USB_VENDOR_ID_LOGITECH:
		if (hdev->product >= USB_DEVICE_ID_LOGITECH_HARMONY_FIRST &&
				hdev->product <= USB_DEVICE_ID_LOGITECH_HARMONY_LAST)
			return true;
		/*
		 * The Keene FM transmitter USB device has the same USB ID as
		 * the Logitech AudioHub Speaker, but it should ignore the hid.
		 * Check if the name is that of the Keene device.
		 * For reference: the name of the AudioHub is
		 * "HOLTEK  AudioHub Speaker".
		 */
		if (hdev->product == USB_DEVICE_ID_LOGITECH_AUDIOHUB &&
			!strcmp(hdev->name, "HOLTEK  B-LINK USB Audio  "))
				return true;
		break;
	case USB_VENDOR_ID_SOUNDGRAPH:
		if (hdev->product >= USB_DEVICE_ID_SOUNDGRAPH_IMON_FIRST &&
		    hdev->product <= USB_DEVICE_ID_SOUNDGRAPH_IMON_LAST)
			return true;
		break;
	case USB_VENDOR_ID_HANWANG:
		if (hdev->product >= USB_DEVICE_ID_HANWANG_TABLET_FIRST &&
		    hdev->product <= USB_DEVICE_ID_HANWANG_TABLET_LAST)
			return true;
		break;
	case USB_VENDOR_ID_JESS:
		if (hdev->product == USB_DEVICE_ID_JESS_YUREX &&
				hdev->type == HID_TYPE_USBNONE)
			return true;
		break;
	case USB_VENDOR_ID_VELLEMAN:
		/* These are not HID devices.  They are handled by comedi. */
		if ((hdev->product >= USB_DEVICE_ID_VELLEMAN_K8055_FIRST &&
		     hdev->product <= USB_DEVICE_ID_VELLEMAN_K8055_LAST) ||
		    (hdev->product >= USB_DEVICE_ID_VELLEMAN_K8061_FIRST &&
		     hdev->product <= USB_DEVICE_ID_VELLEMAN_K8061_LAST))
			return true;
		break;
	case USB_VENDOR_ID_ATMEL_V_USB:
		/* Masterkit MA901 usb radio based on Atmel tiny85 chip and
		 * it has the same USB ID as many Atmel V-USB devices. This
		 * usb radio is handled by radio-ma901.c driver so we want
		 * ignore the hid. Check the name, bus, product and ignore
		 * if we have MA901 usb radio.
		 */
		if (hdev->product == USB_DEVICE_ID_ATMEL_V_USB &&
			hdev->bus == BUS_USB &&
			strncmp(hdev->name, "www.masterkit.ru MA901", 22) == 0)
			return true;
		break;
	}

	if (hdev->type == HID_TYPE_USBMOUSE &&
			hid_match_id(hdev, hid_mouse_ignore_list))
		return true;

	return !!hid_match_id(hdev, hid_ignore_list);
}