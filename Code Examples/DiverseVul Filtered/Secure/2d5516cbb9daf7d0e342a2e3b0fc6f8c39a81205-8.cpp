static void rt_mutex_init_task(struct task_struct *p)
{
	spin_lock_init(&p->pi_lock);
#ifdef CONFIG_RT_MUTEXES
	plist_head_init(&p->pi_waiters, &p->pi_lock);
	p->pi_blocked_on = NULL;
#endif
}