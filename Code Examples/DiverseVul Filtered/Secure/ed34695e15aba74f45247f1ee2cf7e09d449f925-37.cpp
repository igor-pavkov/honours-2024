static int ff_layout_write_done_cb(struct rpc_task *task,
				struct nfs_pgio_header *hdr)
{
	loff_t end_offs = 0;
	int err;

	if (task->tk_status < 0) {
		ff_layout_io_track_ds_error(hdr->lseg, hdr->pgio_mirror_idx,
					    hdr->args.offset, hdr->args.count,
					    &hdr->res.op_status, OP_WRITE,
					    task->tk_status);
		trace_ff_layout_write_error(hdr);
	}

	err = ff_layout_async_handle_error(task, hdr->args.context->state,
					   hdr->ds_clp, hdr->lseg,
					   hdr->pgio_mirror_idx);

	trace_nfs4_pnfs_write(hdr, err);
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
		return -EAGAIN;
	}

	if (hdr->res.verf->committed == NFS_FILE_SYNC ||
	    hdr->res.verf->committed == NFS_DATA_SYNC)
		end_offs = hdr->mds_offset + (loff_t)hdr->res.count;

	/* Note: if the write is unstable, don't set end_offs until commit */
	ff_layout_set_layoutcommit(hdr->inode, hdr->lseg, end_offs);

	/* zero out fattr since we don't care DS attr at all */
	hdr->fattr.valid = 0;
	if (task->tk_status >= 0)
		nfs_writeback_update_inode(hdr);

	return 0;
}