static inline void init_timeout(struct aio_timeout *to)
{
	setup_timer_on_stack(&to->timer, timeout_func, (unsigned long) to);
	to->timed_out = 0;
	to->p = current;
}