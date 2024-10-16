int paste_selection(struct tty_struct *tty)
{
	struct vc_data *vc = tty->driver_data;
	int	pasted = 0;
	unsigned int count;
	struct  tty_ldisc *ld;
	DECLARE_WAITQUEUE(wait, current);

	acquire_console_sem();
	poke_blanked_console();
	release_console_sem();

	ld = tty_ldisc_ref_wait(tty);
	
	add_wait_queue(&vc->paste_wait, &wait);
	while (sel_buffer && sel_buffer_lth > pasted) {
		set_current_state(TASK_INTERRUPTIBLE);
		if (test_bit(TTY_THROTTLED, &tty->flags)) {
			schedule();
			continue;
		}
		count = sel_buffer_lth - pasted;
		count = min(count, tty->receive_room);
		tty->ldisc.ops->receive_buf(tty, sel_buffer + pasted,
								NULL, count);
		pasted += count;
	}
	remove_wait_queue(&vc->paste_wait, &wait);
	__set_current_state(TASK_RUNNING);

	tty_ldisc_deref(ld);
	return 0;
}