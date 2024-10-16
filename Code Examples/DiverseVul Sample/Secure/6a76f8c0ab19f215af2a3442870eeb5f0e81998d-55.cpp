static int alloc_retstack_tasklist(struct ftrace_ret_stack **ret_stack_list)
{
	int i;
	int ret = 0;
	unsigned long flags;
	int start = 0, end = FTRACE_RETSTACK_ALLOC_SIZE;
	struct task_struct *g, *t;

	for (i = 0; i < FTRACE_RETSTACK_ALLOC_SIZE; i++) {
		ret_stack_list[i] = kmalloc(FTRACE_RETFUNC_DEPTH
					* sizeof(struct ftrace_ret_stack),
					GFP_KERNEL);
		if (!ret_stack_list[i]) {
			start = 0;
			end = i;
			ret = -ENOMEM;
			goto free;
		}
	}

	read_lock_irqsave(&tasklist_lock, flags);
	do_each_thread(g, t) {
		if (start == end) {
			ret = -EAGAIN;
			goto unlock;
		}

		if (t->ret_stack == NULL) {
			atomic_set(&t->tracing_graph_pause, 0);
			atomic_set(&t->trace_overrun, 0);
			t->curr_ret_stack = -1;
			/* Make sure the tasks see the -1 first: */
			smp_wmb();
			t->ret_stack = ret_stack_list[start++];
		}
	} while_each_thread(g, t);

unlock:
	read_unlock_irqrestore(&tasklist_lock, flags);
free:
	for (i = start; i < end; i++)
		kfree(ret_stack_list[i]);
	return ret;
}