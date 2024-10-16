static int smsusb1_load_firmware(struct usb_device *udev, int id, int board_id)
{
	const struct firmware *fw;
	u8 *fw_buffer;
	int rc, dummy;
	char *fw_filename;

	if (id < 0)
		id = sms_get_board(board_id)->default_mode;

	if (id < DEVICE_MODE_DVBT || id > DEVICE_MODE_DVBT_BDA) {
		pr_err("invalid firmware id specified %d\n", id);
		return -EINVAL;
	}

	fw_filename = sms_get_fw_name(id, board_id);

	rc = request_firmware(&fw, fw_filename, &udev->dev);
	if (rc < 0) {
		pr_warn("failed to open '%s' mode %d, trying again with default firmware\n",
			fw_filename, id);

		fw_filename = smsusb1_fw_lkup[id];
		rc = request_firmware(&fw, fw_filename, &udev->dev);
		if (rc < 0) {
			pr_warn("failed to open '%s' mode %d\n",
				 fw_filename, id);

			return rc;
		}
	}

	fw_buffer = kmalloc(fw->size, GFP_KERNEL);
	if (fw_buffer) {
		memcpy(fw_buffer, fw->data, fw->size);

		rc = usb_bulk_msg(udev, usb_sndbulkpipe(udev, 2),
				  fw_buffer, fw->size, &dummy, 1000);

		pr_debug("sent %zu(%d) bytes, rc %d\n", fw->size, dummy, rc);

		kfree(fw_buffer);
	} else {
		pr_err("failed to allocate firmware buffer\n");
		rc = -ENOMEM;
	}
	pr_debug("read FW %s, size=%zu\n", fw_filename, fw->size);

	release_firmware(fw);

	return rc;
}