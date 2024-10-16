static inline struct thread_info *alloc_thread_info(struct task_struct *tsk)
{
#ifdef CONFIG_DEBUG_STACK_USAGE
	gfp_t mask = GFP_KERNEL | __GFP_ZERO;
#else
	gfp_t mask = GFP_KERNEL;
#endif
	return (struct thread_info *)__get_free_pages(mask, THREAD_SIZE_ORDER);
}