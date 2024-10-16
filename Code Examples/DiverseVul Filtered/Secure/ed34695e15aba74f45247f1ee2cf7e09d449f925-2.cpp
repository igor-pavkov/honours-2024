static void ff_layout_commit_record_layoutstats_start(struct rpc_task *task,
		struct nfs_commit_data *cdata)
{
	if (test_and_set_bit(NFS_IOHDR_STAT, &cdata->flags))
		return;
	nfs4_ff_layout_stat_io_start_write(cdata->inode,
			FF_LAYOUT_COMP(cdata->lseg, cdata->ds_commit_index),
			0, task->tk_start);
}