asmlinkage long sys_vhangup(void)
{
	if (capable(CAP_SYS_TTY_CONFIG)) {
		/* XXX: this needs locking */
		tty_vhangup(current->signal->tty);
		return 0;
	}
	return -EPERM;
}