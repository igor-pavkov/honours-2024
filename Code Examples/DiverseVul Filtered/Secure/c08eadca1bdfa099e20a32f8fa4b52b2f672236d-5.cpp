static int em28xx_init_dev(struct em28xx *dev, struct usb_device *udev,
			   struct usb_interface *intf,
			   int minor)
{
	int retval;
	const char *chip_name = NULL;

	dev->intf = intf;
	mutex_init(&dev->ctrl_urb_lock);
	spin_lock_init(&dev->slock);

	dev->em28xx_write_regs = em28xx_write_regs;
	dev->em28xx_read_reg = em28xx_read_reg;
	dev->em28xx_read_reg_req_len = em28xx_read_reg_req_len;
	dev->em28xx_write_regs_req = em28xx_write_regs_req;
	dev->em28xx_read_reg_req = em28xx_read_reg_req;
	dev->board.is_em2800 = em28xx_boards[dev->model].is_em2800;

	em28xx_set_model(dev);

	dev->wait_after_write = 5;

	/* Based on the Chip ID, set the device configuration */
	retval = em28xx_read_reg(dev, EM28XX_R0A_CHIPID);
	if (retval > 0) {
		dev->chip_id = retval;

		switch (dev->chip_id) {
		case CHIP_ID_EM2800:
			chip_name = "em2800";
			break;
		case CHIP_ID_EM2710:
			chip_name = "em2710";
			break;
		case CHIP_ID_EM2750:
			chip_name = "em2750";
			break;
		case CHIP_ID_EM2765:
			chip_name = "em2765";
			dev->wait_after_write = 0;
			dev->is_em25xx = 1;
			dev->eeprom_addrwidth_16bit = 1;
			break;
		case CHIP_ID_EM2820:
			chip_name = "em2710/2820";
			if (le16_to_cpu(udev->descriptor.idVendor) == 0xeb1a) {
				__le16 idProd = udev->descriptor.idProduct;

				if (le16_to_cpu(idProd) == 0x2710)
					chip_name = "em2710";
				else if (le16_to_cpu(idProd) == 0x2820)
					chip_name = "em2820";
			}
			/* NOTE: the em2820 is used in webcams, too ! */
			break;
		case CHIP_ID_EM2840:
			chip_name = "em2840";
			break;
		case CHIP_ID_EM2860:
			chip_name = "em2860";
			break;
		case CHIP_ID_EM2870:
			chip_name = "em2870";
			dev->wait_after_write = 0;
			break;
		case CHIP_ID_EM2874:
			chip_name = "em2874";
			dev->wait_after_write = 0;
			dev->eeprom_addrwidth_16bit = 1;
			break;
		case CHIP_ID_EM28174:
			chip_name = "em28174";
			dev->wait_after_write = 0;
			dev->eeprom_addrwidth_16bit = 1;
			break;
		case CHIP_ID_EM28178:
			chip_name = "em28178";
			dev->wait_after_write = 0;
			dev->eeprom_addrwidth_16bit = 1;
			break;
		case CHIP_ID_EM2883:
			chip_name = "em2882/3";
			dev->wait_after_write = 0;
			break;
		case CHIP_ID_EM2884:
			chip_name = "em2884";
			dev->wait_after_write = 0;
			dev->eeprom_addrwidth_16bit = 1;
			break;
		}
	}
	if (!chip_name)
		dev_info(&dev->intf->dev,
			 "unknown em28xx chip ID (%d)\n", dev->chip_id);
	else
		dev_info(&dev->intf->dev, "chip ID is %s\n", chip_name);

	em28xx_media_device_init(dev, udev);

	if (dev->is_audio_only) {
		retval = em28xx_audio_setup(dev);
		if (retval) {
			retval = -ENODEV;
			goto err_deinit_media;
		}
		em28xx_init_extension(dev);

		return 0;
	}

	em28xx_pre_card_setup(dev);

	rt_mutex_init(&dev->i2c_bus_lock);

	/* register i2c bus 0 */
	if (dev->board.is_em2800)
		retval = em28xx_i2c_register(dev, 0, EM28XX_I2C_ALGO_EM2800);
	else
		retval = em28xx_i2c_register(dev, 0, EM28XX_I2C_ALGO_EM28XX);
	if (retval < 0) {
		dev_err(&dev->intf->dev,
			"%s: em28xx_i2c_register bus 0 - error [%d]!\n",
		       __func__, retval);
		goto err_deinit_media;
	}

	/* register i2c bus 1 */
	if (dev->def_i2c_bus) {
		if (dev->is_em25xx)
			retval = em28xx_i2c_register(dev, 1,
						     EM28XX_I2C_ALGO_EM25XX_BUS_B);
		else
			retval = em28xx_i2c_register(dev, 1,
						     EM28XX_I2C_ALGO_EM28XX);
		if (retval < 0) {
			dev_err(&dev->intf->dev,
				"%s: em28xx_i2c_register bus 1 - error [%d]!\n",
				__func__, retval);

			goto err_unreg_i2c;
		}
	}

	/* Do board specific init and eeprom reading */
	em28xx_card_setup(dev);

	return 0;

err_unreg_i2c:
	em28xx_i2c_unregister(dev, 0);
err_deinit_media:
	em28xx_unregister_media_device(dev);
	return retval;
}