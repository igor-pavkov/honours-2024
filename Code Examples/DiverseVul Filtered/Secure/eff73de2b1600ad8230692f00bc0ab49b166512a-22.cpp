static int configure_transfer_mode(struct camera_data *cam, unsigned int alt)
{
	static unsigned char iso_regs[8][4] = {
		{0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00},
		{0xB9, 0x00, 0x00, 0x7E},
		{0xB9, 0x00, 0x01, 0x7E},
		{0xB9, 0x00, 0x02, 0x7E},
		{0xB9, 0x00, 0x02, 0xFE},
		{0xB9, 0x00, 0x03, 0x7E},
		{0xB9, 0x00, 0x03, 0xFD}
	};
	struct cpia2_command cmd;
	unsigned char reg;

	if (!video_is_registered(&cam->vdev))
		return -ENODEV;

	/***
	 * Write the isoc registers according to the alternate selected
	 ***/
	cmd.direction = TRANSFER_WRITE;
	cmd.buffer.block_data[0] = iso_regs[alt][0];
	cmd.buffer.block_data[1] = iso_regs[alt][1];
	cmd.buffer.block_data[2] = iso_regs[alt][2];
	cmd.buffer.block_data[3] = iso_regs[alt][3];
	cmd.req_mode = CAMERAACCESS_TYPE_BLOCK | CAMERAACCESS_VC;
	cmd.start = CPIA2_VC_USB_ISOLIM;
	cmd.reg_count = 4;
	cpia2_send_command(cam, &cmd);

	/***
	 * Enable relevant streams before starting polling.
	 * First read USB Stream Config Register.
	 ***/
	cmd.direction = TRANSFER_READ;
	cmd.req_mode = CAMERAACCESS_TYPE_BLOCK | CAMERAACCESS_VC;
	cmd.start = CPIA2_VC_USB_STRM;
	cmd.reg_count = 1;
	cpia2_send_command(cam, &cmd);
	reg = cmd.buffer.block_data[0];

	/* Clear iso, bulk, and int */
	reg &= ~(CPIA2_VC_USB_STRM_BLK_ENABLE |
		 CPIA2_VC_USB_STRM_ISO_ENABLE |
		 CPIA2_VC_USB_STRM_INT_ENABLE);

	if (alt == USBIF_BULK) {
		DBG("Enabling bulk xfer\n");
		reg |= CPIA2_VC_USB_STRM_BLK_ENABLE;	/* Enable Bulk */
		cam->xfer_mode = XFER_BULK;
	} else if (alt >= USBIF_ISO_1) {
		DBG("Enabling ISOC xfer\n");
		reg |= CPIA2_VC_USB_STRM_ISO_ENABLE;
		cam->xfer_mode = XFER_ISOC;
	}

	cmd.buffer.block_data[0] = reg;
	cmd.direction = TRANSFER_WRITE;
	cmd.start = CPIA2_VC_USB_STRM;
	cmd.reg_count = 1;
	cmd.req_mode = CAMERAACCESS_TYPE_BLOCK | CAMERAACCESS_VC;
	cpia2_send_command(cam, &cmd);

	return 0;
}