static void n_tty_flush_buffer(struct tty_struct *tty)
{
	down_write(&tty->termios_rwsem);
	reset_buffer_flags(tty->disc_data);
	n_tty_set_room(tty);

	if (tty->link)
		n_tty_packet_mode_flush(tty);
	up_write(&tty->termios_rwsem);
}