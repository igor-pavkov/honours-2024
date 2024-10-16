static inline void destroy_context(struct mm_struct *mm)
{
	destroy_context_ldt(mm);
}