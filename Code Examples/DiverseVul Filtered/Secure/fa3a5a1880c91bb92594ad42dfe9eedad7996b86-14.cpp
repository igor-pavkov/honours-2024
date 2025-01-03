static void ml_ff_destroy(struct ff_device *ff)
{
	struct ml_device *ml = ff->private;

	/*
	 * Even though we stop all playing effects when tearing down
	 * an input device (via input_device_flush() that calls into
	 * input_ff_flush() that stops and erases all effects), we
	 * do not actually stop the timer, and therefore we should
	 * do it here.
	 */
	del_timer_sync(&ml->timer);

	kfree(ml->private);
}