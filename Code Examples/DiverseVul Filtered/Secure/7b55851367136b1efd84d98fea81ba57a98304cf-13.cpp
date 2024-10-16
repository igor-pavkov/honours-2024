static unsigned long *alloc_thread_stack_node(struct task_struct *tsk, int node)
{
#ifdef CONFIG_VMAP_STACK
	void *stack;
	int i;

	for (i = 0; i < NR_CACHED_STACKS; i++) {
		struct vm_struct *s;

		s = this_cpu_xchg(cached_stacks[i], NULL);

		if (!s)
			continue;

		/* Clear stale pointers from reused stack. */
		memset(s->addr, 0, THREAD_SIZE);

		tsk->stack_vm_area = s;
		return s->addr;
	}

	/*
	 * Allocated stacks are cached and later reused by new threads,
	 * so memcg accounting is performed manually on assigning/releasing
	 * stacks to tasks. Drop __GFP_ACCOUNT.
	 */
	stack = __vmalloc_node_range(THREAD_SIZE, THREAD_ALIGN,
				     VMALLOC_START, VMALLOC_END,
				     THREADINFO_GFP & ~__GFP_ACCOUNT,
				     PAGE_KERNEL,
				     0, node, __builtin_return_address(0));

	/*
	 * We can't call find_vm_area() in interrupt context, and
	 * free_thread_stack() can be called in interrupt context,
	 * so cache the vm_struct.
	 */
	if (stack) {
		tsk->stack_vm_area = find_vm_area(stack);
		tsk->stack = stack;
	}
	return stack;
#else
	struct page *page = alloc_pages_node(node, THREADINFO_GFP,
					     THREAD_SIZE_ORDER);

	return page ? page_address(page) : NULL;
#endif
}