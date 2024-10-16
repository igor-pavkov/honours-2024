void acct_update_integrals(struct task_struct *tsk)
{
	if (likely(tsk->mm)) {
		long delta = cputime_to_jiffies(
			cputime_sub(tsk->stime, tsk->acct_stimexpd));

		if (delta == 0)
			return;
		tsk->acct_stimexpd = tsk->stime;
		tsk->acct_rss_mem1 += delta * get_mm_rss(tsk->mm);
		tsk->acct_vm_mem1 += delta * tsk->mm->total_vm;
	}
}