void giveup_vsx(struct task_struct *tsk)
{
	giveup_fpu_maybe_transactional(tsk);
	giveup_altivec_maybe_transactional(tsk);
	__giveup_vsx(tsk);
}