static int memcg_charge_kernel_stack(struct task_struct *tsk)
{
#ifdef CONFIG_VMAP_STACK
	struct vm_struct *vm = task_stack_vm_area(tsk);
	int ret;

	if (vm) {
		int i;

		for (i = 0; i < THREAD_SIZE / PAGE_SIZE; i++) {
			/*
			 * If memcg_kmem_charge() fails, page->mem_cgroup
			 * pointer is NULL, and both memcg_kmem_uncharge()
			 * and mod_memcg_page_state() in free_thread_stack()
			 * will ignore this page. So it's safe.
			 */
			ret = memcg_kmem_charge(vm->pages[i], GFP_KERNEL, 0);
			if (ret)
				return ret;

			mod_memcg_page_state(vm->pages[i],
					     MEMCG_KERNEL_STACK_KB,
					     PAGE_SIZE / 1024);
		}
	}
#endif
	return 0;
}