static int technisat_usb2_frontend_attach(struct dvb_usb_adapter *a)
{
	struct usb_device *udev = a->dev->udev;
	int ret;

	a->fe_adap[0].fe = dvb_attach(stv090x_attach, &technisat_usb2_stv090x_config,
			&a->dev->i2c_adap, STV090x_DEMODULATOR_0);

	if (a->fe_adap[0].fe) {
		const struct stv6110x_devctl *ctl;

		ctl = dvb_attach(stv6110x_attach,
				a->fe_adap[0].fe,
				&technisat_usb2_stv6110x_config,
				&a->dev->i2c_adap);

		if (ctl) {
			technisat_usb2_stv090x_config.tuner_init          = ctl->tuner_init;
			technisat_usb2_stv090x_config.tuner_sleep         = ctl->tuner_sleep;
			technisat_usb2_stv090x_config.tuner_set_mode      = ctl->tuner_set_mode;
			technisat_usb2_stv090x_config.tuner_set_frequency = ctl->tuner_set_frequency;
			technisat_usb2_stv090x_config.tuner_get_frequency = ctl->tuner_get_frequency;
			technisat_usb2_stv090x_config.tuner_set_bandwidth = ctl->tuner_set_bandwidth;
			technisat_usb2_stv090x_config.tuner_get_bandwidth = ctl->tuner_get_bandwidth;
			technisat_usb2_stv090x_config.tuner_set_bbgain    = ctl->tuner_set_bbgain;
			technisat_usb2_stv090x_config.tuner_get_bbgain    = ctl->tuner_get_bbgain;
			technisat_usb2_stv090x_config.tuner_set_refclk    = ctl->tuner_set_refclk;
			technisat_usb2_stv090x_config.tuner_get_status    = ctl->tuner_get_status;

			/* call the init function once to initialize
			   tuner's clock output divider and demod's
			   master clock */
			if (a->fe_adap[0].fe->ops.init)
				a->fe_adap[0].fe->ops.init(a->fe_adap[0].fe);

			if (mutex_lock_interruptible(&a->dev->i2c_mutex) < 0)
				return -EAGAIN;

			ret = usb_control_msg(udev, usb_sndctrlpipe(udev, 0),
					SET_IFCLK_TO_EXTERNAL_TSCLK_VENDOR_REQUEST,
					USB_TYPE_VENDOR | USB_DIR_OUT,
					0, 0,
					NULL, 0, 500);
			mutex_unlock(&a->dev->i2c_mutex);

			if (ret != 0)
				err("could not set IF_CLK to external");

			a->fe_adap[0].fe->ops.set_voltage = technisat_usb2_set_voltage;

			/* if everything was successful assign a nice name to the frontend */
			strscpy(a->fe_adap[0].fe->ops.info.name,
				a->dev->desc->name,
				sizeof(a->fe_adap[0].fe->ops.info.name));
		} else {
			dvb_frontend_detach(a->fe_adap[0].fe);
			a->fe_adap[0].fe = NULL;
		}
	}

	technisat_usb2_set_led_timer(a->dev, 1, 1);

	return a->fe_adap[0].fe == NULL ? -ENODEV : 0;
}