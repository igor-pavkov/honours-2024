static void __init multi_register_ports(struct uart_driver *drv)
{
	int i;

	multi_init_ports();

	for (i = 0; i < NR_PORTS; i++) {
		struct mp_port *mtpt = &multi_ports[i];

		mtpt->port.line = i;
		mtpt->port.ops = &multi_pops;
		init_timer(&mtpt->timer);
		mtpt->timer.function = multi_timeout;
		mp_add_one_port(drv, &mtpt->port);
	}
}