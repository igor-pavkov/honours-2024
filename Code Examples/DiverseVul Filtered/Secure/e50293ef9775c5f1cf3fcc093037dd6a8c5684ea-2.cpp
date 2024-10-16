static void hub_event(struct work_struct *work)
{
	struct usb_device *hdev;
	struct usb_interface *intf;
	struct usb_hub *hub;
	struct device *hub_dev;
	u16 hubstatus;
	u16 hubchange;
	int i, ret;

	hub = container_of(work, struct usb_hub, events);
	hdev = hub->hdev;
	hub_dev = hub->intfdev;
	intf = to_usb_interface(hub_dev);

	dev_dbg(hub_dev, "state %d ports %d chg %04x evt %04x\n",
			hdev->state, hdev->maxchild,
			/* NOTE: expects max 15 ports... */
			(u16) hub->change_bits[0],
			(u16) hub->event_bits[0]);

	/* Lock the device, then check to see if we were
	 * disconnected while waiting for the lock to succeed. */
	usb_lock_device(hdev);
	if (unlikely(hub->disconnected))
		goto out_hdev_lock;

	/* If the hub has died, clean up after it */
	if (hdev->state == USB_STATE_NOTATTACHED) {
		hub->error = -ENODEV;
		hub_quiesce(hub, HUB_DISCONNECT);
		goto out_hdev_lock;
	}

	/* Autoresume */
	ret = usb_autopm_get_interface(intf);
	if (ret) {
		dev_dbg(hub_dev, "Can't autoresume: %d\n", ret);
		goto out_hdev_lock;
	}

	/* If this is an inactive hub, do nothing */
	if (hub->quiescing)
		goto out_autopm;

	if (hub->error) {
		dev_dbg(hub_dev, "resetting for error %d\n", hub->error);

		ret = usb_reset_device(hdev);
		if (ret) {
			dev_dbg(hub_dev, "error resetting hub: %d\n", ret);
			goto out_autopm;
		}

		hub->nerrors = 0;
		hub->error = 0;
	}

	/* deal with port status changes */
	for (i = 1; i <= hdev->maxchild; i++) {
		struct usb_port *port_dev = hub->ports[i - 1];

		if (test_bit(i, hub->event_bits)
				|| test_bit(i, hub->change_bits)
				|| test_bit(i, hub->wakeup_bits)) {
			/*
			 * The get_noresume and barrier ensure that if
			 * the port was in the process of resuming, we
			 * flush that work and keep the port active for
			 * the duration of the port_event().  However,
			 * if the port is runtime pm suspended
			 * (powered-off), we leave it in that state, run
			 * an abbreviated port_event(), and move on.
			 */
			pm_runtime_get_noresume(&port_dev->dev);
			pm_runtime_barrier(&port_dev->dev);
			usb_lock_port(port_dev);
			port_event(hub, i);
			usb_unlock_port(port_dev);
			pm_runtime_put_sync(&port_dev->dev);
		}
	}

	/* deal with hub status changes */
	if (test_and_clear_bit(0, hub->event_bits) == 0)
		;	/* do nothing */
	else if (hub_hub_status(hub, &hubstatus, &hubchange) < 0)
		dev_err(hub_dev, "get_hub_status failed\n");
	else {
		if (hubchange & HUB_CHANGE_LOCAL_POWER) {
			dev_dbg(hub_dev, "power change\n");
			clear_hub_feature(hdev, C_HUB_LOCAL_POWER);
			if (hubstatus & HUB_STATUS_LOCAL_POWER)
				/* FIXME: Is this always true? */
				hub->limited_power = 1;
			else
				hub->limited_power = 0;
		}
		if (hubchange & HUB_CHANGE_OVERCURRENT) {
			u16 status = 0;
			u16 unused;

			dev_dbg(hub_dev, "over-current change\n");
			clear_hub_feature(hdev, C_HUB_OVER_CURRENT);
			msleep(500);	/* Cool down */
			hub_power_on(hub, true);
			hub_hub_status(hub, &status, &unused);
			if (status & HUB_STATUS_OVERCURRENT)
				dev_err(hub_dev, "over-current condition\n");
		}
	}

out_autopm:
	/* Balance the usb_autopm_get_interface() above */
	usb_autopm_put_interface_no_suspend(intf);
out_hdev_lock:
	usb_unlock_device(hdev);

	/* Balance the stuff in kick_hub_wq() and allow autosuspend */
	usb_autopm_put_interface(intf);
	kref_put(&hub->kref, hub_release);
}