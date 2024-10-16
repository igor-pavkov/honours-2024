static int rfcomm_tty_tiocmset(struct tty_struct *tty, unsigned int set, unsigned int clear)
{
	struct rfcomm_dev *dev = (struct rfcomm_dev *) tty->driver_data;
	struct rfcomm_dlc *dlc = dev->dlc;
	u8 v24_sig;

	BT_DBG("tty %p dev %p set 0x%02x clear 0x%02x", tty, dev, set, clear);

	rfcomm_dlc_get_modem_status(dlc, &v24_sig);

	if (set & TIOCM_DSR || set & TIOCM_DTR)
		v24_sig |= RFCOMM_V24_RTC;
	if (set & TIOCM_RTS || set & TIOCM_CTS)
		v24_sig |= RFCOMM_V24_RTR;
	if (set & TIOCM_RI)
		v24_sig |= RFCOMM_V24_IC;
	if (set & TIOCM_CD)
		v24_sig |= RFCOMM_V24_DV;

	if (clear & TIOCM_DSR || clear & TIOCM_DTR)
		v24_sig &= ~RFCOMM_V24_RTC;
	if (clear & TIOCM_RTS || clear & TIOCM_CTS)
		v24_sig &= ~RFCOMM_V24_RTR;
	if (clear & TIOCM_RI)
		v24_sig &= ~RFCOMM_V24_IC;
	if (clear & TIOCM_CD)
		v24_sig &= ~RFCOMM_V24_DV;

	rfcomm_dlc_set_modem_status(dlc, v24_sig);

	return 0;
}