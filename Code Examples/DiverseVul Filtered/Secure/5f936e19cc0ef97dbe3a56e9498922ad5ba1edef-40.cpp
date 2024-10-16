static enum alarmtimer_restart alarmtimer_nsleep_wakeup(struct alarm *alarm,
								ktime_t now)
{
	struct task_struct *task = (struct task_struct *)alarm->data;

	alarm->data = NULL;
	if (task)
		wake_up_process(task);
	return ALARMTIMER_NORESTART;
}