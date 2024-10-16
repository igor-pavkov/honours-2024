void __put_task_struct(struct task_struct *tsk)
{
	WARN_ON(!tsk->exit_state);
	WARN_ON(atomic_read(&tsk->usage));
	WARN_ON(tsk == current);

	put_cred(tsk->real_cred);
	put_cred(tsk->cred);
	delayacct_tsk_free(tsk);

	if (!profile_handoff_task(tsk))
		free_task(tsk);
}