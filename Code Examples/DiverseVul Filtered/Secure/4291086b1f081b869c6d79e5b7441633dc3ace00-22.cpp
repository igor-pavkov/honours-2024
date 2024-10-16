static void n_tty_receive_buf(struct tty_struct *tty, const unsigned char *cp,
			      char *fp, int count)
{
	n_tty_receive_buf_common(tty, cp, fp, count, 0);
}