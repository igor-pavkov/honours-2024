static inline void load_LDT_nolock(mm_context_t *pc)
{
	set_ldt(pc->ldt, pc->size);
}