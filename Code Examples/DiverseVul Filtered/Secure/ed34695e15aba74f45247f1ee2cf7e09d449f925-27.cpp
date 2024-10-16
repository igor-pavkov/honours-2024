static void ff_layout_read_record_layoutstats_start(struct rpc_task *task,
		struct nfs_pgio_header *hdr)
{
	if (test_and_set_bit(NFS_IOHDR_STAT, &hdr->flags))
		return;
	nfs4_ff_layout_stat_io_start_read(hdr->inode,
			FF_LAYOUT_COMP(hdr->lseg, hdr->pgio_mirror_idx),
			hdr->args.count,
			task->tk_start);
}