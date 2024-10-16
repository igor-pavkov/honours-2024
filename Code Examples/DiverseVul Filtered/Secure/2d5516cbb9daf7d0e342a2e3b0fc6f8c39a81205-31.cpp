static void cleanup_signal(struct task_struct *tsk)
{
	struct signal_struct *sig = tsk->signal;

	atomic_dec(&sig->live);

	if (atomic_dec_and_test(&sig->count))
		__cleanup_signal(sig);
}