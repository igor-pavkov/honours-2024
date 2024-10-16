static inline int valid_irq_stack(unsigned long sp, struct task_struct *p,
				  unsigned long nbytes)
{
	unsigned long stack_page;
	unsigned long cpu = task_cpu(p);

	stack_page = (unsigned long)hardirq_ctx[cpu];
	if (sp >= stack_page && sp <= stack_page + THREAD_SIZE - nbytes)
		return 1;

	stack_page = (unsigned long)softirq_ctx[cpu];
	if (sp >= stack_page && sp <= stack_page + THREAD_SIZE - nbytes)
		return 1;

	return 0;
}