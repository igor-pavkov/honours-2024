void hrtimer_init_sleeper(struct hrtimer_sleeper *sl, struct task_struct *task)
{
	sl->timer.function = hrtimer_wakeup;
	sl->task = task;
#ifdef CONFIG_HIGH_RES_TIMERS
	sl->timer.cb_mode = HRTIMER_CB_IRQSAFE_NO_RESTART;
#endif
}