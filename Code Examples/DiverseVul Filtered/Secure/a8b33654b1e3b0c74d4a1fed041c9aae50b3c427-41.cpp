static void mp_set_termios(struct tty_struct *tty, struct MP_TERMIOS *old_termios)
{
	struct sb_uart_state *state = tty->driver_data;
	unsigned long flags;
	unsigned int cflag = tty->termios.c_cflag;

#define RELEVANT_IFLAG(iflag)	((iflag) & (IGNBRK|BRKINT|IGNPAR|PARMRK|INPCK))

	if ((cflag ^ old_termios->c_cflag) == 0 &&
			RELEVANT_IFLAG(tty->termios.c_iflag ^ old_termios->c_iflag) == 0)
		return;

	mp_change_speed(state, old_termios);

	if ((old_termios->c_cflag & CBAUD) && !(cflag & CBAUD))
		uart_clear_mctrl(state->port, TIOCM_RTS | TIOCM_DTR);

	if (!(old_termios->c_cflag & CBAUD) && (cflag & CBAUD)) {
		unsigned int mask = TIOCM_DTR;
		if (!(cflag & CRTSCTS) ||
				!test_bit(TTY_THROTTLED, &tty->flags))
			mask |= TIOCM_RTS;
		uart_set_mctrl(state->port, mask);
	}

	if ((old_termios->c_cflag & CRTSCTS) && !(cflag & CRTSCTS)) {
		spin_lock_irqsave(&state->port->lock, flags);
		tty->hw_stopped = 0;
		__mp_start(tty);
		spin_unlock_irqrestore(&state->port->lock, flags);
	}

	if (!(old_termios->c_cflag & CRTSCTS) && (cflag & CRTSCTS)) {
		spin_lock_irqsave(&state->port->lock, flags);
		if (!(state->port->ops->get_mctrl(state->port) & TIOCM_CTS)) {
			tty->hw_stopped = 1;
			state->port->ops->stop_tx(state->port);
		}
		spin_unlock_irqrestore(&state->port->lock, flags);
	}
}