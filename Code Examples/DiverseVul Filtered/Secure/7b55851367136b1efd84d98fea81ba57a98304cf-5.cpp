static inline void free_thread_stack(struct task_struct *tsk)
{
#ifdef CONFIG_VMAP_STACK
	struct vm_struct *vm = task_stack_vm_area(tsk);

	if (vm) {
		int i;

		for (i = 0; i < THREAD_SIZE / PAGE_SIZE; i++) {
			mod_memcg_page_state(vm->pages[i],
					     MEMCG_KERNEL_STACK_KB,
					     -(int)(PAGE_SIZE / 1024));

			memcg_kmem_uncharge(vm->pages[i], 0);
		}

		for (i = 0; i < NR_CACHED_STACKS; i++) {
			if (this_cpu_cmpxchg(cached_stacks[i],
					NULL, tsk->stack_vm_area) != NULL)
				continue;

			return;
		}

		vfree_atomic(tsk->stack);
		return;
	}
#endif

	__free_pages(virt_to_page(tsk->stack), THREAD_SIZE_ORDER);
}