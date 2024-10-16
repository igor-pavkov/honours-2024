static void uvesafb_reset(struct uvesafb_ktask *task)
{
	struct completion *cpl = task->done;

	memset(task, 0, sizeof(*task));
	task->done = cpl;
}