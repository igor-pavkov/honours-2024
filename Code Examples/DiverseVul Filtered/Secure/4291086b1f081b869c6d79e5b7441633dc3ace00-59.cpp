static void n_tty_receive_char(struct tty_struct *tty, unsigned char c)
{
	n_tty_receive_char_inline(tty, c);
}