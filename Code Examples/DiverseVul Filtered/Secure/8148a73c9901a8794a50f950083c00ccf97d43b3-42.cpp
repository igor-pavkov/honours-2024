static int show_timer(struct seq_file *m, void *v)
{
	struct k_itimer *timer;
	struct timers_private *tp = m->private;
	int notify;
	static const char * const nstr[] = {
		[SIGEV_SIGNAL] = "signal",
		[SIGEV_NONE] = "none",
		[SIGEV_THREAD] = "thread",
	};

	timer = list_entry((struct list_head *)v, struct k_itimer, list);
	notify = timer->it_sigev_notify;

	seq_printf(m, "ID: %d\n", timer->it_id);
	seq_printf(m, "signal: %d/%p\n",
		   timer->sigq->info.si_signo,
		   timer->sigq->info.si_value.sival_ptr);
	seq_printf(m, "notify: %s/%s.%d\n",
		   nstr[notify & ~SIGEV_THREAD_ID],
		   (notify & SIGEV_THREAD_ID) ? "tid" : "pid",
		   pid_nr_ns(timer->it_pid, tp->ns));
	seq_printf(m, "ClockID: %d\n", timer->it_clock);

	return 0;
}