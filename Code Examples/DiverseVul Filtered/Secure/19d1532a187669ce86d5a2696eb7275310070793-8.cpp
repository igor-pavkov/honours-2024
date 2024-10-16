static void sixpack_receive_buf(struct tty_struct *tty,
	const unsigned char *cp, const char *fp, int count)
{
	struct sixpack *sp;
	int count1;

	if (!count)
		return;

	sp = sp_get(tty);
	if (!sp)
		return;

	/* Read the characters out of the buffer */
	count1 = count;
	while (count) {
		count--;
		if (fp && *fp++) {
			if (!test_and_set_bit(SIXPF_ERROR, &sp->flags))
				sp->dev->stats.rx_errors++;
			continue;
		}
	}
	sixpack_decode(sp, cp, count1);

	sp_put(sp);
	tty_unthrottle(tty);
}