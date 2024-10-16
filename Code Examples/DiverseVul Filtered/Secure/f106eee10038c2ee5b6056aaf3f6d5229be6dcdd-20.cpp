void free_task(struct task_struct *tsk)
{
	prop_local_destroy_single(&tsk->dirties);
	account_kernel_stack(tsk->stack, -1);
	free_thread_info(tsk->stack);
	rt_mutex_debug_task_free(tsk);
	ftrace_graph_exit_task(tsk);
	free_task_struct(tsk);
}