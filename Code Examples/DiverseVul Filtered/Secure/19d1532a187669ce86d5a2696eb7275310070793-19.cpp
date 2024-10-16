static void decode_std_command(struct sixpack *sp, unsigned char cmd)
{
	unsigned char checksum = 0, rest = 0;
	short i;

	switch (cmd & SIXP_CMD_MASK) {     /* normal command */
	case SIXP_SEOF:
		if ((sp->rx_count == 0) && (sp->rx_count_cooked == 0)) {
			if ((sp->status & SIXP_RX_DCD_MASK) ==
				SIXP_RX_DCD_MASK) {
				sp->led_state = 0x68;
				sp->tty->ops->write(sp->tty, &sp->led_state, 1);
			}
		} else {
			sp->led_state = 0x60;
			/* fill trailing bytes with zeroes */
			sp->tty->ops->write(sp->tty, &sp->led_state, 1);
			rest = sp->rx_count;
			if (rest != 0)
				 for (i = rest; i <= 3; i++)
					decode_data(sp, 0);
			if (rest == 2)
				sp->rx_count_cooked -= 2;
			else if (rest == 3)
				sp->rx_count_cooked -= 1;
			for (i = 0; i < sp->rx_count_cooked; i++)
				checksum += sp->cooked_buf[i];
			if (checksum != SIXP_CHKSUM) {
				printk(KERN_DEBUG "6pack: bad checksum %2.2x\n", checksum);
			} else {
				sp->rcount = sp->rx_count_cooked-2;
				sp_bump(sp, 0);
			}
			sp->rx_count_cooked = 0;
		}
		break;
	case SIXP_TX_URUN: printk(KERN_DEBUG "6pack: TX underrun\n");
		break;
	case SIXP_RX_ORUN: printk(KERN_DEBUG "6pack: RX overrun\n");
		break;
	case SIXP_RX_BUF_OVL:
		printk(KERN_DEBUG "6pack: RX buffer overflow\n");
	}
}