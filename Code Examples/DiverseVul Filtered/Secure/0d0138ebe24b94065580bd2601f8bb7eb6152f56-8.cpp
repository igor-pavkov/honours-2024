void user_enable_single_step(struct task_struct *child)
{
	child->ptrace |= PT_SINGLESTEP;
}