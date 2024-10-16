static void ff_layout_commit_record_layoutstats_done(struct rpc_task *task,
		struct nfs_commit_data *cdata)
{
	struct nfs_page *req;
	__u64 count = 0;

	if (!test_and_clear_bit(NFS_IOHDR_STAT, &cdata->flags))
		return;

	if (task->tk_status == 0) {
		list_for_each_entry(req, &cdata->pages, wb_list)
			count += req->wb_bytes;
	}
	nfs4_ff_layout_stat_io_end_write(task,
			FF_LAYOUT_COMP(cdata->lseg, cdata->ds_commit_index),
			count, count, NFS_FILE_SYNC);
	set_bit(NFS_LSEG_LAYOUTRETURN, &cdata->lseg->pls_flags);
}