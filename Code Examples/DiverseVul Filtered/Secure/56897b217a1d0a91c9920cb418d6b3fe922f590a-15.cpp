void hci_uart_set_speeds(struct hci_uart *hu, unsigned int init_speed,
			 unsigned int oper_speed)
{
	hu->init_speed = init_speed;
	hu->oper_speed = oper_speed;
}