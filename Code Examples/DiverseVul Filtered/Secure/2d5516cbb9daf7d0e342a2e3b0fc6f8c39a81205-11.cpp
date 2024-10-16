static int copy_io(unsigned long clone_flags, struct task_struct *tsk)
{
#ifdef CONFIG_BLOCK
	struct io_context *ioc = current->io_context;

	if (!ioc)
		return 0;
	/*
	 * Share io context with parent, if CLONE_IO is set
	 */
	if (clone_flags & CLONE_IO) {
		tsk->io_context = ioc_task_link(ioc);
		if (unlikely(!tsk->io_context))
			return -ENOMEM;
	} else if (ioprio_valid(ioc->ioprio)) {
		tsk->io_context = alloc_io_context(GFP_KERNEL, -1);
		if (unlikely(!tsk->io_context))
			return -ENOMEM;

		tsk->io_context->ioprio = ioc->ioprio;
	}
#endif
	return 0;
}