static void ff_layout_write_record_layoutstats_done(struct rpc_task *task,
		struct nfs_pgio_header *hdr)
{
	if (!test_and_clear_bit(NFS_IOHDR_STAT, &hdr->flags))
		return;
	nfs4_ff_layout_stat_io_end_write(task,
			FF_LAYOUT_COMP(hdr->lseg, hdr->pgio_mirror_idx),
			hdr->args.count, hdr->res.count,
			hdr->res.verf->committed);
	set_bit(NFS_LSEG_LAYOUTRETURN, &hdr->lseg->pls_flags);
}