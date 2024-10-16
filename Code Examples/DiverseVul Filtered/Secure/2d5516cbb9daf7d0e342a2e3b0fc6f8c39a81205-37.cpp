void free_task(struct task_struct *tsk)
{
	prop_local_destroy_single(&tsk->dirties);
	free_thread_info(tsk->stack);
	rt_mutex_debug_task_free(tsk);
	ftrace_graph_exit_task(tsk);
	free_task_struct(tsk);
}