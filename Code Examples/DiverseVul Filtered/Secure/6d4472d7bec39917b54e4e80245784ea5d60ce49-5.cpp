int hiddev_connect(struct hid_device *hid, unsigned int force)
{
	struct hiddev *hiddev;
	struct usbhid_device *usbhid = hid->driver_data;
	int retval;

	if (!force) {
		unsigned int i;
		for (i = 0; i < hid->maxcollection; i++)
			if (hid->collection[i].type ==
			    HID_COLLECTION_APPLICATION &&
			    !IS_INPUT_APPLICATION(hid->collection[i].usage))
				break;

		if (i == hid->maxcollection)
			return -1;
	}

	if (!(hiddev = kzalloc(sizeof(struct hiddev), GFP_KERNEL)))
		return -1;

	init_waitqueue_head(&hiddev->wait);
	INIT_LIST_HEAD(&hiddev->list);
	spin_lock_init(&hiddev->list_lock);
	mutex_init(&hiddev->existancelock);
	hid->hiddev = hiddev;
	hiddev->hid = hid;
	hiddev->exist = 1;
	retval = usb_register_dev(usbhid->intf, &hiddev_class);
	if (retval) {
		hid_err(hid, "Not able to get a minor for this device\n");
		hid->hiddev = NULL;
		kfree(hiddev);
		return -1;
	}

	/*
	 * If HID_QUIRK_NO_INIT_REPORTS is set, make sure we don't initialize
	 * the reports.
	 */
	hiddev->initialized = hid->quirks & HID_QUIRK_NO_INIT_REPORTS;

	hiddev->minor = usbhid->intf->minor;

	return 0;
}