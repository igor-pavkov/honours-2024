static int em28xx_hint_board(struct em28xx *dev)
{
	int i;

	if (dev->is_webcam) {
		if (dev->em28xx_sensor == EM28XX_MT9V011) {
			dev->model = EM2820_BOARD_SILVERCREST_WEBCAM;
		} else if (dev->em28xx_sensor == EM28XX_MT9M001 ||
			   dev->em28xx_sensor == EM28XX_MT9M111) {
			dev->model = EM2750_BOARD_UNKNOWN;
		}
		/* FIXME: IMPROVE ! */

		return 0;
	}

	/*
	 * HINT method: EEPROM
	 *
	 * This method works only for boards with eeprom.
	 * Uses a hash of all eeprom bytes. The hash should be
	 * unique for a vendor/tuner pair.
	 * There are a high chance that tuners for different
	 * video standards produce different hashes.
	 */
	for (i = 0; i < ARRAY_SIZE(em28xx_eeprom_hash); i++) {
		if (dev->hash == em28xx_eeprom_hash[i].hash) {
			dev->model = em28xx_eeprom_hash[i].model;
			dev->tuner_type = em28xx_eeprom_hash[i].tuner;

			dev_err(&dev->intf->dev,
				"Your board has no unique USB ID.\n"
				"A hint were successfully done, based on eeprom hash.\n"
				"This method is not 100%% failproof.\n"
				"If the board were misdetected, please email this log to:\n"
				"\tV4L Mailing List  <linux-media@vger.kernel.org>\n"
				"Board detected as %s\n",
			       em28xx_boards[dev->model].name);

			return 0;
		}
	}

	/*
	 * HINT method: I2C attached devices
	 *
	 * This method works for all boards.
	 * Uses a hash of i2c scanned devices.
	 * Devices with the same i2c attached chips will
	 * be considered equal.
	 * This method is less precise than the eeprom one.
	 */

	/* user did not request i2c scanning => do it now */
	if (!dev->i2c_hash)
		em28xx_do_i2c_scan(dev, dev->def_i2c_bus);

	for (i = 0; i < ARRAY_SIZE(em28xx_i2c_hash); i++) {
		if (dev->i2c_hash == em28xx_i2c_hash[i].hash) {
			dev->model = em28xx_i2c_hash[i].model;
			dev->tuner_type = em28xx_i2c_hash[i].tuner;
			dev_err(&dev->intf->dev,
				"Your board has no unique USB ID.\n"
				"A hint were successfully done, based on i2c devicelist hash.\n"
				"This method is not 100%% failproof.\n"
				"If the board were misdetected, please email this log to:\n"
				"\tV4L Mailing List  <linux-media@vger.kernel.org>\n"
				"Board detected as %s\n",
				em28xx_boards[dev->model].name);

			return 0;
		}
	}

	dev_err(&dev->intf->dev,
		"Your board has no unique USB ID and thus need a hint to be detected.\n"
		"You may try to use card=<n> insmod option to workaround that.\n"
		"Please send an email with this log to:\n"
		"\tV4L Mailing List <linux-media@vger.kernel.org>\n"
		"Board eeprom hash is 0x%08lx\n"
		"Board i2c devicelist hash is 0x%08lx\n",
		dev->hash, dev->i2c_hash);

	dev_err(&dev->intf->dev,
		"Here is a list of valid choices for the card=<n> insmod option:\n");
	for (i = 0; i < em28xx_bcount; i++) {
		dev_err(&dev->intf->dev,
			"    card=%d -> %s\n", i, em28xx_boards[i].name);
	}
	return -1;
}