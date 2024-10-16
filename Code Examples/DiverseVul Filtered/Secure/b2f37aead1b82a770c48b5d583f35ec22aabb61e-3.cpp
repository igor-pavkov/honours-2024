static struct mkiss *mkiss_get(struct tty_struct *tty)
{
	struct mkiss *ax;

	read_lock(&disc_data_lock);
	ax = tty->disc_data;
	if (ax)
		refcount_inc(&ax->refcnt);
	read_unlock(&disc_data_lock);

	return ax;
}