static void commit_echoes(struct tty_struct *tty)
{
	struct n_tty_data *ldata = tty->disc_data;
	size_t nr, old, echoed;
	size_t head;

	head = ldata->echo_head;
	ldata->echo_mark = head;
	old = ldata->echo_commit - ldata->echo_tail;

	/* Process committed echoes if the accumulated # of bytes
	 * is over the threshold (and try again each time another
	 * block is accumulated) */
	nr = head - ldata->echo_tail;
	if (nr < ECHO_COMMIT_WATERMARK || (nr % ECHO_BLOCK > old % ECHO_BLOCK))
		return;

	mutex_lock(&ldata->output_lock);
	ldata->echo_commit = head;
	echoed = __process_echoes(tty);
	mutex_unlock(&ldata->output_lock);

	if (echoed && tty->ops->flush_chars)
		tty->ops->flush_chars(tty);
}