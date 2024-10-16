static void fib6_gc_timer_cb(unsigned long arg)
{
	fib6_run_gc(0, (struct net *)arg);
}