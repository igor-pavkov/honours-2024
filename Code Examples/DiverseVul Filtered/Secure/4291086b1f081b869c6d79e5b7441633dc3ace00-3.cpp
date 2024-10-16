static inline int tty_put_user(struct tty_struct *tty, unsigned char x,
			       unsigned char __user *ptr)
{
	struct n_tty_data *ldata = tty->disc_data;

	tty_audit_add_data(tty, &x, 1, ldata->icanon);
	return put_user(x, ptr);
}