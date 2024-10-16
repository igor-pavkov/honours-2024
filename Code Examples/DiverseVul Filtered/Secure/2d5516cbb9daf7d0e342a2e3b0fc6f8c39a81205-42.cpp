static inline void mm_free_pgd(struct mm_struct * mm)
{
	pgd_free(mm, mm->pgd);
}