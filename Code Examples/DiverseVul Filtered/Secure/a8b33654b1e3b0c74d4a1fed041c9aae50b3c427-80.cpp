static inline void mp_report_port(struct uart_driver *drv, struct sb_uart_port *port)
{
	char address[64];

	switch (port->iotype) {
		case UPIO_PORT:
			snprintf(address, sizeof(address),"I/O 0x%x", port->iobase);
			break;
		case UPIO_HUB6:
			snprintf(address, sizeof(address),"I/O 0x%x offset 0x%x", port->iobase, port->hub6);
			break;
		case UPIO_MEM:
			snprintf(address, sizeof(address),"MMIO 0x%lx", port->mapbase);
			break;
		default:
			snprintf(address, sizeof(address),"*unknown*" );
			strlcpy(address, "*unknown*", sizeof(address));
			break;
	}

	printk( "%s%d at %s (irq = %d) is a %s\n",
			drv->dev_name, port->line, address, port->irq, mp_type(port));

}