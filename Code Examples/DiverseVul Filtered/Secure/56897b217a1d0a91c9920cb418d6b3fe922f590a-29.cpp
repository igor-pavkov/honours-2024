static int hci_uart_tty_open(struct tty_struct *tty)
{
	struct hci_uart *hu;

	BT_DBG("tty %p", tty);

	/* Error if the tty has no write op instead of leaving an exploitable
	 * hole
	 */
	if (tty->ops->write == NULL)
		return -EOPNOTSUPP;

	hu = kzalloc(sizeof(struct hci_uart), GFP_KERNEL);
	if (!hu) {
		BT_ERR("Can't allocate control structure");
		return -ENFILE;
	}

	tty->disc_data = hu;
	hu->tty = tty;
	tty->receive_room = 65536;

	/* disable alignment support by default */
	hu->alignment = 1;
	hu->padding = 0;

	INIT_WORK(&hu->init_ready, hci_uart_init_work);
	INIT_WORK(&hu->write_work, hci_uart_write_work);

	percpu_init_rwsem(&hu->proto_lock);

	/* Flush any pending characters in the driver */
	tty_driver_flush_buffer(tty);

	return 0;
}