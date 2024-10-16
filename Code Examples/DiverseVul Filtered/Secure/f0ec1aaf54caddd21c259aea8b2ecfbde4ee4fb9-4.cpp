void acct_clear_integrals(struct task_struct *tsk)
{
	tsk->acct_stimexpd = 0;
	tsk->acct_rss_mem1 = 0;
	tsk->acct_vm_mem1 = 0;
}