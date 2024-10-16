static ssize_t n_tty_chars_in_buffer(struct tty_struct *tty)
{
	ssize_t n;

	WARN_ONCE(1, "%s is deprecated and scheduled for removal.", __func__);

	down_write(&tty->termios_rwsem);
	n = chars_in_buffer(tty);
	up_write(&tty->termios_rwsem);
	return n;
}