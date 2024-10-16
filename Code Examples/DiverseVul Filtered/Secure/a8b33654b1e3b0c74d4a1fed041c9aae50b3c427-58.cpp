static void mp_hangup(struct tty_struct *tty)
{
	struct sb_uart_state *state = tty->driver_data;

	MP_STATE_LOCK(state);
	if (state->info && state->info->flags & UIF_NORMAL_ACTIVE) {
		mp_flush_buffer(tty);
		mp_shutdown(state);
		state->count = 0;
		state->info->flags &= ~UIF_NORMAL_ACTIVE;
		state->info->tty = NULL;
		wake_up_interruptible(&state->info->open_wait);
		wake_up_interruptible(&state->info->delta_msr_wait);
	}
	MP_STATE_UNLOCK(state);
}