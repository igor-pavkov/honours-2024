static void ff_layout_commit_done(struct rpc_task *task, void *data)
{
	pnfs_generic_write_commit_done(task, data);
}