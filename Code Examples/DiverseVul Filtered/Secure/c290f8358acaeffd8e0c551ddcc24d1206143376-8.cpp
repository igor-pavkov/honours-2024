static void release_one_tty(struct work_struct *work)
{
	struct tty_struct *tty =
		container_of(work, struct tty_struct, hangup_work);
	struct tty_driver *driver = tty->driver;

	if (tty->ops->cleanup)
		tty->ops->cleanup(tty);

	tty->magic = 0;
	tty_driver_kref_put(driver);
	module_put(driver->owner);

	spin_lock(&tty_files_lock);
	list_del_init(&tty->tty_files);
	spin_unlock(&tty_files_lock);

	put_pid(tty->pgrp);
	put_pid(tty->session);
	free_tty_struct(tty);
}