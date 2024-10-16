void tty_write_message(struct tty_struct *tty, char *msg)
{
	if (tty) {
		mutex_lock(&tty->atomic_write_lock);
		tty_lock();
		if (tty->ops->write && !test_bit(TTY_CLOSING, &tty->flags)) {
			tty_unlock();
			tty->ops->write(tty, msg, strlen(msg));
		} else
			tty_unlock();
		tty_write_unlock(tty);
	}
	return;
}