static void stub_disconnect(struct usb_device *udev)
{
	struct stub_device *sdev;
	const char *udev_busid = dev_name(&udev->dev);
	struct bus_id_priv *busid_priv;
	int rc;

	dev_dbg(&udev->dev, "Enter disconnect\n");

	busid_priv = get_busid_priv(udev_busid);
	if (!busid_priv) {
		BUG();
		return;
	}

	sdev = dev_get_drvdata(&udev->dev);

	/* get stub_device */
	if (!sdev) {
		dev_err(&udev->dev, "could not get device");
		/* release busid_lock */
		put_busid_priv(busid_priv);
		return;
	}

	dev_set_drvdata(&udev->dev, NULL);

	/* release busid_lock before call to remove device files */
	put_busid_priv(busid_priv);

	/*
	 * NOTE: rx/tx threads are invoked for each usb_device.
	 */

	/* release port */
	rc = usb_hub_release_port(udev->parent, udev->portnum,
				  (struct usb_dev_state *) udev);
	if (rc) {
		dev_dbg(&udev->dev, "unable to release port\n");
		return;
	}

	/* If usb reset is called from event handler */
	if (usbip_in_eh(current))
		return;

	/* we already have busid_priv, just lock busid_lock */
	spin_lock(&busid_priv->busid_lock);
	if (!busid_priv->shutdown_busid)
		busid_priv->shutdown_busid = 1;
	/* release busid_lock */
	spin_unlock(&busid_priv->busid_lock);

	/* shutdown the current connection */
	shutdown_busid(busid_priv);

	usb_put_dev(sdev->udev);

	/* we already have busid_priv, just lock busid_lock */
	spin_lock(&busid_priv->busid_lock);
	/* free sdev */
	busid_priv->sdev = NULL;
	stub_device_free(sdev);

	if (busid_priv->status == STUB_BUSID_ALLOC)
		busid_priv->status = STUB_BUSID_ADDED;
	/* release busid_lock */
	spin_unlock(&busid_priv->busid_lock);
	return;
}