int validate_sp(unsigned long sp, struct task_struct *p,
		       unsigned long nbytes)
{
	unsigned long stack_page = (unsigned long)task_stack_page(p);

	if (sp >= stack_page + sizeof(struct thread_struct)
	    && sp <= stack_page + THREAD_SIZE - nbytes)
		return 1;

	return valid_irq_stack(sp, p, nbytes);
}