int set_thread_tidr(struct task_struct *t)
{
	if (!cpu_has_feature(CPU_FTR_P9_TIDR))
		return -EINVAL;

	if (t != current)
		return -EINVAL;

	if (t->thread.tidr)
		return 0;

	t->thread.tidr = (u16)task_pid_nr(t);
	mtspr(SPRN_TIDR, t->thread.tidr);

	return 0;
}