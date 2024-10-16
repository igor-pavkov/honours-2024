static int atusb_probe(struct usb_interface *interface,
		       const struct usb_device_id *id)
{
	struct usb_device *usb_dev = interface_to_usbdev(interface);
	struct ieee802154_hw *hw;
	struct atusb *atusb = NULL;
	int ret = -ENOMEM;

	hw = ieee802154_alloc_hw(sizeof(struct atusb), &atusb_ops);
	if (!hw)
		return -ENOMEM;

	atusb = hw->priv;
	atusb->hw = hw;
	atusb->usb_dev = usb_get_dev(usb_dev);
	usb_set_intfdata(interface, atusb);

	atusb->shutdown = 0;
	atusb->err = 0;
	INIT_DELAYED_WORK(&atusb->work, atusb_work_urbs);
	init_usb_anchor(&atusb->idle_urbs);
	init_usb_anchor(&atusb->rx_urbs);

	if (atusb_alloc_urbs(atusb, ATUSB_NUM_RX_URBS))
		goto fail;

	atusb->tx_dr.bRequestType = ATUSB_REQ_TO_DEV;
	atusb->tx_dr.bRequest = ATUSB_TX;
	atusb->tx_dr.wValue = cpu_to_le16(0);

	atusb->tx_urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!atusb->tx_urb)
		goto fail;

	hw->parent = &usb_dev->dev;

	atusb_command(atusb, ATUSB_RF_RESET, 0);
	atusb_get_and_conf_chip(atusb);
	atusb_get_and_show_revision(atusb);
	atusb_get_and_show_build(atusb);
	atusb_set_extended_addr(atusb);

	if ((atusb->fw_ver_maj == 0 && atusb->fw_ver_min >= 3) || atusb->fw_ver_maj > 0)
		hw->flags |= IEEE802154_HW_FRAME_RETRIES;

	ret = atusb_get_and_clear_error(atusb);
	if (ret) {
		dev_err(&atusb->usb_dev->dev,
			"%s: initialization failed, error = %d\n",
			__func__, ret);
		goto fail;
	}

	ret = ieee802154_register_hw(hw);
	if (ret)
		goto fail;

	/* If we just powered on, we're now in P_ON and need to enter TRX_OFF
	 * explicitly. Any resets after that will send us straight to TRX_OFF,
	 * making the command below redundant.
	 */
	atusb_write_reg(atusb, RG_TRX_STATE, STATE_FORCE_TRX_OFF);
	msleep(1);	/* reset => TRX_OFF, tTR13 = 37 us */

#if 0
	/* Calculating the maximum time available to empty the frame buffer
	 * on reception:
	 *
	 * According to [1], the inter-frame gap is
	 * R * 20 * 16 us + 128 us
	 * where R is a random number from 0 to 7. Furthermore, we have 20 bit
	 * times (80 us at 250 kbps) of SHR of the next frame before the
	 * transceiver begins storing data in the frame buffer.
	 *
	 * This yields a minimum time of 208 us between the last data of a
	 * frame and the first data of the next frame. This time is further
	 * reduced by interrupt latency in the atusb firmware.
	 *
	 * atusb currently needs about 500 us to retrieve a maximum-sized
	 * frame. We therefore have to allow reception of a new frame to begin
	 * while we retrieve the previous frame.
	 *
	 * [1] "JN-AN-1035 Calculating data rates in an IEEE 802.15.4-based
	 *      network", Jennic 2006.
	 *     http://www.jennic.com/download_file.php?supportFile=JN-AN-1035%20Calculating%20802-15-4%20Data%20Rates-1v0.pdf
	 */

	atusb_write_subreg(atusb, SR_RX_SAFE_MODE, 1);
#endif
	atusb_write_reg(atusb, RG_IRQ_MASK, 0xff);

	ret = atusb_get_and_clear_error(atusb);
	if (!ret)
		return 0;

	dev_err(&atusb->usb_dev->dev,
		"%s: setup failed, error = %d\n",
		__func__, ret);

	ieee802154_unregister_hw(hw);
fail:
	atusb_free_urbs(atusb);
	usb_kill_urb(atusb->tx_urb);
	usb_free_urb(atusb->tx_urb);
	usb_put_dev(usb_dev);
	ieee802154_free_hw(hw);
	return ret;
}