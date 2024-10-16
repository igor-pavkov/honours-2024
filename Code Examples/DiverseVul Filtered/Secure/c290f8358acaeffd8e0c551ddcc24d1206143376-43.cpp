static void __proc_set_tty(struct task_struct *tsk, struct tty_struct *tty)
{
	if (tty) {
		unsigned long flags;
		/* We should not have a session or pgrp to put here but.... */
		spin_lock_irqsave(&tty->ctrl_lock, flags);
		put_pid(tty->session);
		put_pid(tty->pgrp);
		tty->pgrp = get_pid(task_pgrp(tsk));
		spin_unlock_irqrestore(&tty->ctrl_lock, flags);
		tty->session = get_pid(task_session(tsk));
		if (tsk->signal->tty) {
			printk(KERN_DEBUG "tty not NULL!!\n");
			tty_kref_put(tsk->signal->tty);
		}
	}
	put_pid(tsk->signal->tty_old_pgrp);
	tsk->signal->tty = tty_kref_get(tty);
	tsk->signal->tty_old_pgrp = NULL;
}