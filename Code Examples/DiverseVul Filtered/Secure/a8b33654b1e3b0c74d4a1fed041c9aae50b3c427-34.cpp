static int multi_verify_port(struct sb_uart_port *port, struct serial_struct *ser)
{
	if (ser->irq >= NR_IRQS || ser->irq < 0 ||
			ser->baud_base < 9600 || ser->type < PORT_UNKNOWN ||
			ser->type == PORT_STARTECH)
		return -EINVAL;
	return 0;
}