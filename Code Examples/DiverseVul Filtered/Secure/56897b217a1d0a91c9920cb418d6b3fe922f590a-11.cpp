static int __init hci_uart_init(void)
{
	static struct tty_ldisc_ops hci_uart_ldisc;
	int err;

	BT_INFO("HCI UART driver ver %s", VERSION);

	/* Register the tty discipline */

	memset(&hci_uart_ldisc, 0, sizeof(hci_uart_ldisc));
	hci_uart_ldisc.magic		= TTY_LDISC_MAGIC;
	hci_uart_ldisc.name		= "n_hci";
	hci_uart_ldisc.open		= hci_uart_tty_open;
	hci_uart_ldisc.close		= hci_uart_tty_close;
	hci_uart_ldisc.read		= hci_uart_tty_read;
	hci_uart_ldisc.write		= hci_uart_tty_write;
	hci_uart_ldisc.ioctl		= hci_uart_tty_ioctl;
	hci_uart_ldisc.compat_ioctl	= hci_uart_tty_ioctl;
	hci_uart_ldisc.poll		= hci_uart_tty_poll;
	hci_uart_ldisc.receive_buf	= hci_uart_tty_receive;
	hci_uart_ldisc.write_wakeup	= hci_uart_tty_wakeup;
	hci_uart_ldisc.owner		= THIS_MODULE;

	err = tty_register_ldisc(N_HCI, &hci_uart_ldisc);
	if (err) {
		BT_ERR("HCI line discipline registration failed. (%d)", err);
		return err;
	}

#ifdef CONFIG_BT_HCIUART_H4
	h4_init();
#endif
#ifdef CONFIG_BT_HCIUART_BCSP
	bcsp_init();
#endif
#ifdef CONFIG_BT_HCIUART_LL
	ll_init();
#endif
#ifdef CONFIG_BT_HCIUART_ATH3K
	ath_init();
#endif
#ifdef CONFIG_BT_HCIUART_3WIRE
	h5_init();
#endif
#ifdef CONFIG_BT_HCIUART_INTEL
	intel_init();
#endif
#ifdef CONFIG_BT_HCIUART_BCM
	bcm_init();
#endif
#ifdef CONFIG_BT_HCIUART_QCA
	qca_init();
#endif
#ifdef CONFIG_BT_HCIUART_AG6XX
	ag6xx_init();
#endif
#ifdef CONFIG_BT_HCIUART_MRVL
	mrvl_init();
#endif

	return 0;
}