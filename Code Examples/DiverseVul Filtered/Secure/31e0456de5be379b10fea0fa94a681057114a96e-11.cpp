static int smsusb_start_streaming(struct smsusb_device_t *dev)
{
	int i, rc;

	for (i = 0; i < MAX_URBS; i++) {
		rc = smsusb_submit_urb(dev, &dev->surbs[i]);
		if (rc < 0) {
			pr_err("smsusb_submit_urb(...) failed\n");
			smsusb_stop_streaming(dev);
			break;
		}
	}

	return rc;
}