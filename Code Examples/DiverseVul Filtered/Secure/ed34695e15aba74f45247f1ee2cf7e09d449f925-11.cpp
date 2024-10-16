static void ff_layout_commit_count_stats(struct rpc_task *task, void *data)
{
	struct nfs_commit_data *cdata = data;

	ff_layout_commit_record_layoutstats_done(task, cdata);
	rpc_count_iostats_metrics(task,
	    &NFS_CLIENT(cdata->inode)->cl_metrics[NFSPROC4_CLNT_COMMIT]);
}