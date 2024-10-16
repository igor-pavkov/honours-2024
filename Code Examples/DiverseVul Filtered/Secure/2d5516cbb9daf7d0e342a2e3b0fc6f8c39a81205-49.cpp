void mm_init_owner(struct mm_struct *mm, struct task_struct *p)
{
	mm->owner = p;
}