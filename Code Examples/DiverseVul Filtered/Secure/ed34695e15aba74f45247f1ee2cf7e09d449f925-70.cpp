static int ff_layout_read_done_cb(struct rpc_task *task,
				struct nfs_pgio_header *hdr)
{
	int err;

	if (task->tk_status < 0) {
		ff_layout_io_track_ds_error(hdr->lseg, hdr->pgio_mirror_idx,
					    hdr->args.offset, hdr->args.count,
					    &hdr->res.op_status, OP_READ,
					    task->tk_status);
		trace_ff_layout_read_error(hdr);
	}

	err = ff_layout_async_handle_error(task, hdr->args.context->state,
					   hdr->ds_clp, hdr->lseg,
					   hdr->pgio_mirror_idx);

	trace_nfs4_pnfs_read(hdr, err);
	clear_bit(NFS_IOHDR_RESEND_PNFS, &hdr->flags);
	clear_bit(NFS_IOHDR_RESEND_MDS, &hdr->flags);
	switch (err) {
	case -NFS4ERR_RESET_TO_PNFS:
		set_bit(NFS_IOHDR_RESEND_PNFS, &hdr->flags);
		return task->tk_status;
	case -NFS4ERR_RESET_TO_MDS:
		set_bit(NFS_IOHDR_RESEND_MDS, &hdr->flags);
		return task->tk_status;
	case -EAGAIN:
		goto out_eagain;
	}

	return 0;
out_eagain:
	rpc_restart_call_prepare(task);
	return -EAGAIN;
}