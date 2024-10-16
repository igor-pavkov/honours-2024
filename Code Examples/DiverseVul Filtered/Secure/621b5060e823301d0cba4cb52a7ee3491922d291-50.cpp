static inline void __switch_to_tm(struct task_struct *prev)
{
	if (cpu_has_feature(CPU_FTR_TM)) {
		tm_enable();
		tm_reclaim_task(prev);
	}
}