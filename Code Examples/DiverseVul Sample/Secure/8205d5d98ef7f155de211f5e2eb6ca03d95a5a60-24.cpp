unsigned long get_wchan(struct task_struct *p)
{
	unsigned long ret;

	if (!try_get_task_stack(p))
		return 0;

	ret = __get_wchan(p);

	put_task_stack(p);

	return ret;
}