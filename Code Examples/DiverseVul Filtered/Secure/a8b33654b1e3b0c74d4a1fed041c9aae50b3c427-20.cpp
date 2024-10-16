static void mp_configure_port(struct uart_driver *drv, struct sb_uart_state *state, struct sb_uart_port *port)
{
	unsigned int flags;


	if (!port->iobase && !port->mapbase && !port->membase)
	{
		DPRINTK("%s error \n",__FUNCTION__);
		return;
	}
	flags = UART_CONFIG_TYPE;
	if (port->flags & UPF_AUTO_IRQ)
		flags |= UART_CONFIG_IRQ;
	if (port->flags & UPF_BOOT_AUTOCONF) {
		port->type = PORT_UNKNOWN;
		port->ops->config_port(port, flags);
	}

	if (port->type != PORT_UNKNOWN) {
		unsigned long flags;

		mp_report_port(drv, port);

		spin_lock_irqsave(&port->lock, flags);
		port->ops->set_mctrl(port, 0);
		spin_unlock_irqrestore(&port->lock, flags);

		mp_change_pm(state, 3);
	}
}