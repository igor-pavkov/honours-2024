static int snd_task_name(struct task_struct *task, char *name, size_t size)
{
	unsigned int idx;

	if (snd_BUG_ON(!task || !name || size < 2))
		return -EINVAL;
	for (idx = 0; idx < sizeof(task->comm) && idx + 1 < size; idx++)
		name[idx] = task->comm[idx];
	name[idx] = '\0';
	return 0;
}