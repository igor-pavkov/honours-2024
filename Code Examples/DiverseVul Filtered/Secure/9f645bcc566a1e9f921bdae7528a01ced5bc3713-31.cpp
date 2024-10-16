static struct uvesafb_ktask *uvesafb_prep(void)
{
	struct uvesafb_ktask *task;

	task = kzalloc(sizeof(*task), GFP_KERNEL);
	if (task) {
		task->done = kzalloc(sizeof(*task->done), GFP_KERNEL);
		if (!task->done) {
			kfree(task);
			task = NULL;
		}
	}
	return task;
}