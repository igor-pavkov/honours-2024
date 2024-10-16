static void kiss_unesc(struct mkiss *ax, unsigned char s)
{
	switch (s) {
	case END:
		/* drop keeptest bit = VSV */
		if (test_bit(AXF_KEEPTEST, &ax->flags))
			clear_bit(AXF_KEEPTEST, &ax->flags);

		if (!test_and_clear_bit(AXF_ERROR, &ax->flags) && (ax->rcount > 2))
			ax_bump(ax);

		clear_bit(AXF_ESCAPE, &ax->flags);
		ax->rcount = 0;
		return;

	case ESC:
		set_bit(AXF_ESCAPE, &ax->flags);
		return;
	case ESC_ESC:
		if (test_and_clear_bit(AXF_ESCAPE, &ax->flags))
			s = ESC;
		break;
	case ESC_END:
		if (test_and_clear_bit(AXF_ESCAPE, &ax->flags))
			s = END;
		break;
	}

	spin_lock_bh(&ax->buflock);
	if (!test_bit(AXF_ERROR, &ax->flags)) {
		if (ax->rcount < ax->buffsize) {
			ax->rbuff[ax->rcount++] = s;
			spin_unlock_bh(&ax->buflock);
			return;
		}

		ax->dev->stats.rx_over_errors++;
		set_bit(AXF_ERROR, &ax->flags);
	}
	spin_unlock_bh(&ax->buflock);
}