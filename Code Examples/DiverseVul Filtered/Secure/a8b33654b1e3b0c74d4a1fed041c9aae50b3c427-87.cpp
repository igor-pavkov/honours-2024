static void multi_config_port(struct sb_uart_port *port, int flags)
{
	struct mp_port *mtpt = (struct mp_port *)port;
	int probeflags = PROBE_ANY;

	if (flags & UART_CONFIG_TYPE)
		autoconfig(mtpt, probeflags);
	if (mtpt->port.type != PORT_UNKNOWN && flags & UART_CONFIG_IRQ)
		autoconfig_irq(mtpt);

	if (mtpt->port.type == PORT_UNKNOWN)
		multi_release_std_resource(mtpt);
}