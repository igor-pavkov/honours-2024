static void em28xx_pre_card_setup(struct em28xx *dev)
{
	/*
	 * Set the initial XCLK and I2C clock values based on the board
	 * definition
	 */
	em28xx_set_xclk_i2c_speed(dev);

	/* request some modules */
	switch (dev->model) {
	case EM2861_BOARD_PLEXTOR_PX_TV100U:
		/* Sets the msp34xx I2S speed */
		dev->i2s_speed = 2048000;
		break;
	case EM2861_BOARD_KWORLD_PVRTV_300U:
	case EM2880_BOARD_KWORLD_DVB_305U:
		em28xx_write_reg(dev, EM2820_R08_GPIO_CTRL, 0x6d);
		usleep_range(10000, 11000);
		em28xx_write_reg(dev, EM2820_R08_GPIO_CTRL, 0x7d);
		usleep_range(10000, 11000);
		break;
	case EM2870_BOARD_COMPRO_VIDEOMATE:
		/*
		 * TODO: someone can do some cleanup here...
		 *	 not everything's needed
		 */
		em28xx_write_reg(dev, EM2880_R04_GPO, 0x00);
		usleep_range(10000, 11000);
		em28xx_write_reg(dev, EM2880_R04_GPO, 0x01);
		usleep_range(10000, 11000);
		em28xx_write_reg(dev, EM2820_R08_GPIO_CTRL, 0xfd);
		msleep(70);
		em28xx_write_reg(dev, EM2820_R08_GPIO_CTRL, 0xfc);
		msleep(70);
		em28xx_write_reg(dev, EM2820_R08_GPIO_CTRL, 0xdc);
		msleep(70);
		em28xx_write_reg(dev, EM2820_R08_GPIO_CTRL, 0xfc);
		msleep(70);
		break;
	case EM2870_BOARD_TERRATEC_XS_MT2060:
		/*
		 * this device needs some gpio writes to get the DVB-T
		 * demod work
		 */
		em28xx_write_reg(dev, EM2820_R08_GPIO_CTRL, 0xfe);
		msleep(70);
		em28xx_write_reg(dev, EM2820_R08_GPIO_CTRL, 0xde);
		msleep(70);
		em28xx_write_reg(dev, EM2820_R08_GPIO_CTRL, 0xfe);
		msleep(70);
		break;
	case EM2870_BOARD_PINNACLE_PCTV_DVB:
		/*
		 * this device needs some gpio writes to get the
		 * DVB-T demod work
		 */
		em28xx_write_reg(dev, EM2820_R08_GPIO_CTRL, 0xfe);
		msleep(70);
		em28xx_write_reg(dev, EM2820_R08_GPIO_CTRL, 0xde);
		msleep(70);
		em28xx_write_reg(dev, EM2820_R08_GPIO_CTRL, 0xfe);
		msleep(70);
		break;
	case EM2820_BOARD_GADMEI_UTV310:
	case EM2820_BOARD_MSI_VOX_USB_2:
		/* enables audio for that devices */
		em28xx_write_reg(dev, EM2820_R08_GPIO_CTRL, 0xfd);
		break;

	case EM2882_BOARD_KWORLD_ATSC_315U:
		em28xx_write_reg(dev, EM2820_R08_GPIO_CTRL, 0xff);
		usleep_range(10000, 11000);
		em28xx_write_reg(dev, EM2820_R08_GPIO_CTRL, 0xfe);
		usleep_range(10000, 11000);
		em28xx_write_reg(dev, EM2880_R04_GPO, 0x00);
		usleep_range(10000, 11000);
		em28xx_write_reg(dev, EM2880_R04_GPO, 0x08);
		usleep_range(10000, 11000);
		break;

	case EM2860_BOARD_KAIOMY_TVNPC_U2:
		em28xx_write_regs(dev, EM28XX_R0F_XCLK, "\x07", 1);
		em28xx_write_regs(dev, EM28XX_R06_I2C_CLK, "\x40", 1);
		em28xx_write_regs(dev, 0x0d, "\x42", 1);
		em28xx_write_regs(dev, 0x08, "\xfd", 1);
		usleep_range(10000, 11000);
		em28xx_write_regs(dev, 0x08, "\xff", 1);
		usleep_range(10000, 11000);
		em28xx_write_regs(dev, 0x08, "\x7f", 1);
		usleep_range(10000, 11000);
		em28xx_write_regs(dev, 0x08, "\x6b", 1);

		break;
	case EM2860_BOARD_EASYCAP:
		em28xx_write_regs(dev, 0x08, "\xf8", 1);
		break;

	case EM2820_BOARD_IODATA_GVMVP_SZ:
		em28xx_write_reg(dev, EM2820_R08_GPIO_CTRL, 0xff);
		msleep(70);
		em28xx_write_reg(dev, EM2820_R08_GPIO_CTRL, 0xf7);
		usleep_range(10000, 11000);
		em28xx_write_reg(dev, EM2820_R08_GPIO_CTRL, 0xfe);
		msleep(70);
		em28xx_write_reg(dev, EM2820_R08_GPIO_CTRL, 0xfd);
		msleep(70);
		break;

	case EM2860_BOARD_TERRATEC_GRABBY:
		/*
		 * HACK?: Ensure AC97 register reading is reliable before
		 * proceeding. In practice, this will wait about 1.6 seconds.
		 */
		em28xx_wait_until_ac97_features_equals(dev, 0x6a90);
		break;
	}

	em28xx_gpio_set(dev, dev->board.tuner_gpio);
	em28xx_set_mode(dev, EM28XX_ANALOG_MODE);

	/* Unlock device */
	em28xx_set_mode(dev, EM28XX_SUSPEND);
}