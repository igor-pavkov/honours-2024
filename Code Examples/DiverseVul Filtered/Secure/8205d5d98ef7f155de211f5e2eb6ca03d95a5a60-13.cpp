void show_stack(struct task_struct *tsk, unsigned long *stack)
{
	unsigned long sp, ip, lr, newsp;
	int count = 0;
	int firstframe = 1;
#ifdef CONFIG_FUNCTION_GRAPH_TRACER
	struct ftrace_ret_stack *ret_stack;
	extern void return_to_handler(void);
	unsigned long rth = (unsigned long)return_to_handler;
	int curr_frame = 0;
#endif

	if (tsk == NULL)
		tsk = current;

	if (!try_get_task_stack(tsk))
		return;

	sp = (unsigned long) stack;
	if (sp == 0) {
		if (tsk == current)
			sp = current_stack_pointer();
		else
			sp = tsk->thread.ksp;
	}

	lr = 0;
	printk("Call Trace:\n");
	do {
		if (!validate_sp(sp, tsk, STACK_FRAME_OVERHEAD))
			break;

		stack = (unsigned long *) sp;
		newsp = stack[0];
		ip = stack[STACK_FRAME_LR_SAVE];
		if (!firstframe || ip != lr) {
			printk("["REG"] ["REG"] %pS", sp, ip, (void *)ip);
#ifdef CONFIG_FUNCTION_GRAPH_TRACER
			if ((ip == rth) && curr_frame >= 0) {
				ret_stack = ftrace_graph_get_ret_stack(current,
								  curr_frame++);
				if (ret_stack)
					pr_cont(" (%pS)",
						(void *)ret_stack->ret);
				else
					curr_frame = -1;
			}
#endif
			if (firstframe)
				pr_cont(" (unreliable)");
			pr_cont("\n");
		}
		firstframe = 0;

		/*
		 * See if this is an exception frame.
		 * We look for the "regshere" marker in the current frame.
		 */
		if (validate_sp(sp, tsk, STACK_INT_FRAME_SIZE)
		    && stack[STACK_FRAME_MARKER] == STACK_FRAME_REGS_MARKER) {
			struct pt_regs *regs = (struct pt_regs *)
				(sp + STACK_FRAME_OVERHEAD);
			lr = regs->link;
			printk("--- interrupt: %lx at %pS\n    LR = %pS\n",
			       regs->trap, (void *)regs->nip, (void *)lr);
			firstframe = 1;
		}

		sp = newsp;
	} while (count++ < kstack_depth_to_print);

	put_task_stack(tsk);
}