static void ff_layout_commit_prepare_common(struct rpc_task *task,
		struct nfs_commit_data *cdata)
{
	ff_layout_commit_record_layoutstats_start(task, cdata);
}