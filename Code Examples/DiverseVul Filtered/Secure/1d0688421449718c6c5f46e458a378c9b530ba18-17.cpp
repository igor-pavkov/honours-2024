static int virtbt_probe(struct virtio_device *vdev)
{
	vq_callback_t *callbacks[VIRTBT_NUM_VQS] = {
		[VIRTBT_VQ_TX] = virtbt_tx_done,
		[VIRTBT_VQ_RX] = virtbt_rx_done,
	};
	const char *names[VIRTBT_NUM_VQS] = {
		[VIRTBT_VQ_TX] = "tx",
		[VIRTBT_VQ_RX] = "rx",
	};
	struct virtio_bluetooth *vbt;
	struct hci_dev *hdev;
	int err;
	__u8 type;

	if (!virtio_has_feature(vdev, VIRTIO_F_VERSION_1))
		return -ENODEV;

	type = virtio_cread8(vdev, offsetof(struct virtio_bt_config, type));

	switch (type) {
	case VIRTIO_BT_CONFIG_TYPE_PRIMARY:
	case VIRTIO_BT_CONFIG_TYPE_AMP:
		break;
	default:
		return -EINVAL;
	}

	vbt = kzalloc(sizeof(*vbt), GFP_KERNEL);
	if (!vbt)
		return -ENOMEM;

	vdev->priv = vbt;
	vbt->vdev = vdev;

	INIT_WORK(&vbt->rx, virtbt_rx_work);

	err = virtio_find_vqs(vdev, VIRTBT_NUM_VQS, vbt->vqs, callbacks,
			      names, NULL);
	if (err)
		return err;

	hdev = hci_alloc_dev();
	if (!hdev) {
		err = -ENOMEM;
		goto failed;
	}

	vbt->hdev = hdev;

	hdev->bus = HCI_VIRTIO;
	hdev->dev_type = type;
	hci_set_drvdata(hdev, vbt);

	hdev->open  = virtbt_open;
	hdev->close = virtbt_close;
	hdev->flush = virtbt_flush;
	hdev->send  = virtbt_send_frame;

	if (virtio_has_feature(vdev, VIRTIO_BT_F_VND_HCI)) {
		__u16 vendor;

		virtio_cread(vdev, struct virtio_bt_config, vendor, &vendor);

		switch (vendor) {
		case VIRTIO_BT_CONFIG_VENDOR_ZEPHYR:
			hdev->manufacturer = 1521;
			hdev->setup = virtbt_setup_zephyr;
			hdev->shutdown = virtbt_shutdown_generic;
			hdev->set_bdaddr = virtbt_set_bdaddr_zephyr;
			break;

		case VIRTIO_BT_CONFIG_VENDOR_INTEL:
			hdev->manufacturer = 2;
			hdev->setup = virtbt_setup_intel;
			hdev->shutdown = virtbt_shutdown_generic;
			hdev->set_bdaddr = virtbt_set_bdaddr_intel;
			set_bit(HCI_QUIRK_STRICT_DUPLICATE_FILTER, &hdev->quirks);
			set_bit(HCI_QUIRK_SIMULTANEOUS_DISCOVERY, &hdev->quirks);
			set_bit(HCI_QUIRK_WIDEBAND_SPEECH_SUPPORTED, &hdev->quirks);
			break;

		case VIRTIO_BT_CONFIG_VENDOR_REALTEK:
			hdev->manufacturer = 93;
			hdev->setup = virtbt_setup_realtek;
			hdev->shutdown = virtbt_shutdown_generic;
			set_bit(HCI_QUIRK_SIMULTANEOUS_DISCOVERY, &hdev->quirks);
			set_bit(HCI_QUIRK_WIDEBAND_SPEECH_SUPPORTED, &hdev->quirks);
			break;
		}
	}

	if (virtio_has_feature(vdev, VIRTIO_BT_F_MSFT_EXT)) {
		__u16 msft_opcode;

		virtio_cread(vdev, struct virtio_bt_config,
			     msft_opcode, &msft_opcode);

		hci_set_msft_opcode(hdev, msft_opcode);
	}

	if (virtio_has_feature(vdev, VIRTIO_BT_F_AOSP_EXT))
		hci_set_aosp_capable(hdev);

	if (hci_register_dev(hdev) < 0) {
		hci_free_dev(hdev);
		err = -EBUSY;
		goto failed;
	}

	return 0;

failed:
	vdev->config->del_vqs(vdev);
	return err;
}