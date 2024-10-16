static void decode_prio_command(struct sixpack *sp, unsigned char cmd)
{
	int actual;

	if ((cmd & SIXP_PRIO_DATA_MASK) != 0) {     /* idle ? */

	/* RX and DCD flags can only be set in the same prio command,
	   if the DCD flag has been set without the RX flag in the previous
	   prio command. If DCD has not been set before, something in the
	   transmission has gone wrong. In this case, RX and DCD are
	   cleared in order to prevent the decode_data routine from
	   reading further data that might be corrupt. */

		if (((sp->status & SIXP_DCD_MASK) == 0) &&
			((cmd & SIXP_RX_DCD_MASK) == SIXP_RX_DCD_MASK)) {
				if (sp->status != 1)
					printk(KERN_DEBUG "6pack: protocol violation\n");
				else
					sp->status = 0;
				cmd &= ~SIXP_RX_DCD_MASK;
		}
		sp->status = cmd & SIXP_PRIO_DATA_MASK;
	} else { /* output watchdog char if idle */
		if ((sp->status2 != 0) && (sp->duplex == 1)) {
			sp->led_state = 0x70;
			sp->tty->ops->write(sp->tty, &sp->led_state, 1);
			sp->tx_enable = 1;
			actual = sp->tty->ops->write(sp->tty, sp->xbuff, sp->status2);
			sp->xleft -= actual;
			sp->xhead += actual;
			sp->led_state = 0x60;
			sp->status2 = 0;

		}
	}

	/* needed to trigger the TNC watchdog */
	sp->tty->ops->write(sp->tty, &sp->led_state, 1);

        /* if the state byte has been received, the TNC is present,
           so the resync timer can be reset. */

	if (sp->tnc_state == TNC_IN_SYNC)
		mod_timer(&sp->resync_t, jiffies + SIXP_INIT_RESYNC_TIMEOUT);

	sp->status1 = cmd & SIXP_PRIO_DATA_MASK;
}