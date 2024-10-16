static void copy_flags(unsigned long clone_flags, struct task_struct *p)
{
	unsigned long new_flags = p->flags;

	new_flags &= ~PF_SUPERPRIV;
	new_flags |= PF_FORKNOEXEC;
	new_flags |= PF_STARTING;
	p->flags = new_flags;
	clear_freeze_flag(p);
}