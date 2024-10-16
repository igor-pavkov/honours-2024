static struct sixpack *sp_get(struct tty_struct *tty)
{
	struct sixpack *sp;

	read_lock(&disc_data_lock);
	sp = tty->disc_data;
	if (sp)
		refcount_inc(&sp->refcnt);
	read_unlock(&disc_data_lock);

	return sp;
}