static void ff_layout_commit_prepare_v3(struct rpc_task *task, void *data)
{
	ff_layout_commit_prepare_common(task, data);
	rpc_call_start(task);
}