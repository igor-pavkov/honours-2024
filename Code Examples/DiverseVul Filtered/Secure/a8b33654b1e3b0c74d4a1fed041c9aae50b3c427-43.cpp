static int mp_register_driver(struct uart_driver *drv)
{
	struct tty_driver *normal = NULL;
	int i, retval;

	drv->state = kmalloc(sizeof(struct sb_uart_state) * drv->nr, GFP_KERNEL);
	retval = -ENOMEM;
	if (!drv->state)
	{
		printk("SB PCI Error: Kernel memory allocation error!\n");
		goto out;
	}
	memset(drv->state, 0, sizeof(struct sb_uart_state) * drv->nr);

	normal = alloc_tty_driver(drv->nr);
	if (!normal)
	{
		printk("SB PCI Error: tty allocation error!\n");
		goto out;
	}

	drv->tty_driver = normal;

	normal->owner           = drv->owner;
	normal->magic		= TTY_DRIVER_MAGIC;
	normal->driver_name     = drv->driver_name;
	normal->name		= drv->dev_name;
	normal->major		= drv->major;
	normal->minor_start	= drv->minor;

	normal->num		= MAX_MP_PORT ; 

	normal->type		= TTY_DRIVER_TYPE_SERIAL;
	normal->subtype		= SERIAL_TYPE_NORMAL;
	normal->init_termios	= tty_std_termios;
	normal->init_termios.c_cflag = B9600 | CS8 | CREAD | HUPCL | CLOCAL;
	normal->flags		= TTY_DRIVER_REAL_RAW | TTY_DRIVER_DYNAMIC_DEV;
	normal->driver_state    = drv;

	tty_set_operations(normal, &mp_ops);

for (i = 0; i < drv->nr; i++) {
	struct sb_uart_state *state = drv->state + i;

	state->close_delay     = 500;   
	state->closing_wait    = 30000; 

	mutex_init(&state->mutex);
	}

	retval = tty_register_driver(normal);
out:
	if (retval < 0) {
		printk("Register tty driver Fail!\n");
		put_tty_driver(normal);
		kfree(drv->state);
	}

	return retval;
}