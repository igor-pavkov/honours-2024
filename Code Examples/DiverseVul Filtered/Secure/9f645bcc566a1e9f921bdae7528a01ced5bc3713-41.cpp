static void uvesafb_free(struct uvesafb_ktask *task)
{
	if (task) {
		kfree(task->done);
		kfree(task);
	}
}