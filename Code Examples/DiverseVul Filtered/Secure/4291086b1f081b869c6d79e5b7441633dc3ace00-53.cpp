static void flush_echoes(struct tty_struct *tty)
{
	struct n_tty_data *ldata = tty->disc_data;

	if ((!L_ECHO(tty) && !L_ECHONL(tty)) ||
	    ldata->echo_commit == ldata->echo_head)
		return;

	mutex_lock(&ldata->output_lock);
	ldata->echo_commit = ldata->echo_head;
	__process_echoes(tty);
	mutex_unlock(&ldata->output_lock);
}