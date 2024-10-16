static int n_tty_receive_buf2(struct tty_struct *tty, const unsigned char *cp,
			      char *fp, int count)
{
	return n_tty_receive_buf_common(tty, cp, fp, count, 1);
}