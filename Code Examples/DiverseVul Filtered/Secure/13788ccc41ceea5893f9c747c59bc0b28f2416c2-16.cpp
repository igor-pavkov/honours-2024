static inline int hrtimer_hres_active(void)
{
	return __get_cpu_var(hrtimer_bases).hres_active;
}