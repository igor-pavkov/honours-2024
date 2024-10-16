static int technisat_usb2_set_voltage(struct dvb_frontend *fe,
				      enum fe_sec_voltage voltage)
{
	int i;
	u8 gpio[3] = { 0 }; /* 0 = 2, 1 = 3, 2 = 4 */

	gpio[2] = 1; /* high - voltage ? */

	switch (voltage) {
	case SEC_VOLTAGE_13:
		gpio[0] = 1;
		break;
	case SEC_VOLTAGE_18:
		gpio[0] = 1;
		gpio[1] = 1;
		break;
	default:
	case SEC_VOLTAGE_OFF:
		break;
	}

	for (i = 0; i < 3; i++)
		if (technisat_usb2_stv090x_config.set_gpio(fe, i+2, 0,
							   gpio[i], 0) != 0)
			return -EREMOTEIO;
	return 0;
}