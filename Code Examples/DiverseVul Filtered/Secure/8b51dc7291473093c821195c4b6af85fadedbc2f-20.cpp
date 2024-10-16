static int rsi_probe(struct usb_interface *pfunction,
		     const struct usb_device_id *id)
{
	struct rsi_hw *adapter;
	struct rsi_91x_usbdev *dev;
	u16 fw_status;
	int status;

	rsi_dbg(INIT_ZONE, "%s: Init function called\n", __func__);

	adapter = rsi_91x_init(dev_oper_mode);
	if (!adapter) {
		rsi_dbg(ERR_ZONE, "%s: Failed to init os intf ops\n",
			__func__);
		return -ENOMEM;
	}
	adapter->rsi_host_intf = RSI_HOST_INTF_USB;

	status = rsi_init_usb_interface(adapter, pfunction);
	if (status) {
		rsi_dbg(ERR_ZONE, "%s: Failed to init usb interface\n",
			__func__);
		goto err;
	}

	rsi_dbg(ERR_ZONE, "%s: Initialized os intf ops\n", __func__);

	if (id && id->idProduct == RSI_USB_PID_9113) {
		rsi_dbg(INIT_ZONE, "%s: 9113 module detected\n", __func__);
		adapter->device_model = RSI_DEV_9113;
	} else if (id && id->idProduct == RSI_USB_PID_9116) {
		rsi_dbg(INIT_ZONE, "%s: 9116 module detected\n", __func__);
		adapter->device_model = RSI_DEV_9116;
	} else {
		rsi_dbg(ERR_ZONE, "%s: Unsupported RSI device id 0x%x\n",
			__func__, id->idProduct);
		goto err1;
	}

	dev = (struct rsi_91x_usbdev *)adapter->rsi_dev;

	status = rsi_usb_reg_read(dev->usbdev, FW_STATUS_REG, &fw_status, 2);
	if (status < 0)
		goto err1;
	else
		fw_status &= 1;

	if (!fw_status) {
		rsi_dbg(INIT_ZONE, "Loading firmware...\n");
		status = rsi_hal_device_init(adapter);
		if (status) {
			rsi_dbg(ERR_ZONE, "%s: Failed in device init\n",
				__func__);
			goto err1;
		}
		rsi_dbg(INIT_ZONE, "%s: Device Init Done\n", __func__);
	}

	status = rsi_rx_urb_submit(adapter, WLAN_EP);
	if (status)
		goto err1;

	if (adapter->priv->coex_mode > 1) {
		status = rsi_rx_urb_submit(adapter, BT_EP);
		if (status)
			goto err1;
	}

	return 0;
err1:
	rsi_deinit_usb_interface(adapter);
err:
	rsi_91x_deinit(adapter);
	rsi_dbg(ERR_ZONE, "%s: Failed in probe...Exiting\n", __func__);
	return status;
}