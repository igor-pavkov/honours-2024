static int ff_layout_commit_done_cb(struct rpc_task *task,
				     struct nfs_commit_data *data)
{
	int err;

	if (task->tk_status < 0) {
		ff_layout_io_track_ds_error(data->lseg, data->ds_commit_index,
					    data->args.offset, data->args.count,
					    &data->res.op_status, OP_COMMIT,
					    task->tk_status);
		trace_ff_layout_commit_error(data);
	}

	err = ff_layout_async_handle_error(task, NULL, data->ds_clp,
					   data->lseg, data->ds_commit_index);

	trace_nfs4_pnfs_commit_ds(data, err);
	switch (err) {
	case -NFS4ERR_RESET_TO_PNFS:
		pnfs_generic_prepare_to_resend_writes(data);
		return -EAGAIN;
	case -NFS4ERR_RESET_TO_MDS:
		pnfs_generic_prepare_to_resend_writes(data);
		return -EAGAIN;
	case -EAGAIN:
		rpc_restart_call_prepare(task);
		return -EAGAIN;
	}

	ff_layout_set_layoutcommit(data->inode, data->lseg, data->lwb);

	return 0;
}