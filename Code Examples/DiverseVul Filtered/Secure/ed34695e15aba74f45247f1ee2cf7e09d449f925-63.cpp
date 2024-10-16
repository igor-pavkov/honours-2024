static void ff_layout_reset_read(struct nfs_pgio_header *hdr)
{
	struct rpc_task *task = &hdr->task;

	pnfs_layoutcommit_inode(hdr->inode, false);
	pnfs_error_mark_layout_for_return(hdr->inode, hdr->lseg);

	if (!test_and_set_bit(NFS_IOHDR_REDO, &hdr->flags)) {
		dprintk("%s Reset task %5u for i/o through MDS "
			"(req %s/%llu, %u bytes @ offset %llu)\n", __func__,
			hdr->task.tk_pid,
			hdr->inode->i_sb->s_id,
			(unsigned long long)NFS_FILEID(hdr->inode),
			hdr->args.count,
			(unsigned long long)hdr->args.offset);

		trace_pnfs_mds_fallback_read_done(hdr->inode,
				hdr->args.offset, hdr->args.count,
				IOMODE_READ, NFS_I(hdr->inode)->layout,
				hdr->lseg);
		task->tk_status = pnfs_read_done_resend_to_mds(hdr);
	}
}