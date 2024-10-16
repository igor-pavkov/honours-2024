static int hci_uart_set_flags(struct hci_uart *hu, unsigned long flags)
{
	unsigned long valid_flags = BIT(HCI_UART_RAW_DEVICE) |
				    BIT(HCI_UART_RESET_ON_INIT) |
				    BIT(HCI_UART_CREATE_AMP) |
				    BIT(HCI_UART_INIT_PENDING) |
				    BIT(HCI_UART_EXT_CONFIG) |
				    BIT(HCI_UART_VND_DETECT);

	if (flags & ~valid_flags)
		return -EINVAL;

	hu->hdev_flags = flags;

	return 0;
}