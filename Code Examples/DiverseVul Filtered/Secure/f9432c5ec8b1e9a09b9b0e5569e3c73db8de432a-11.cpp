int __init rfcomm_init_ttys(void)
{
	int error;

	rfcomm_tty_driver = alloc_tty_driver(RFCOMM_TTY_PORTS);
	if (!rfcomm_tty_driver)
		return -ENOMEM;

	rfcomm_tty_driver->driver_name	= "rfcomm";
	rfcomm_tty_driver->name		= "rfcomm";
	rfcomm_tty_driver->major	= RFCOMM_TTY_MAJOR;
	rfcomm_tty_driver->minor_start	= RFCOMM_TTY_MINOR;
	rfcomm_tty_driver->type		= TTY_DRIVER_TYPE_SERIAL;
	rfcomm_tty_driver->subtype	= SERIAL_TYPE_NORMAL;
	rfcomm_tty_driver->flags	= TTY_DRIVER_REAL_RAW | TTY_DRIVER_DYNAMIC_DEV;
	rfcomm_tty_driver->init_termios	= tty_std_termios;
	rfcomm_tty_driver->init_termios.c_cflag	= B9600 | CS8 | CREAD | HUPCL | CLOCAL;
	rfcomm_tty_driver->init_termios.c_lflag &= ~ICANON;
	tty_set_operations(rfcomm_tty_driver, &rfcomm_ops);

	error = tty_register_driver(rfcomm_tty_driver);
	if (error) {
		BT_ERR("Can't register RFCOMM TTY driver");
		put_tty_driver(rfcomm_tty_driver);
		return error;
	}

	BT_INFO("RFCOMM TTY layer initialized");

	return 0;
}