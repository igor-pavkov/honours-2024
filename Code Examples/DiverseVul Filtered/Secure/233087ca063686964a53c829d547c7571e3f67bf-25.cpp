static inline void debugt(const char *func, const char *msg)
{
	if (drive_params[current_drive].flags & DEBUGT)
		pr_info("%s:%s dtime=%lu\n", func, msg, jiffies - debugtimer);
}