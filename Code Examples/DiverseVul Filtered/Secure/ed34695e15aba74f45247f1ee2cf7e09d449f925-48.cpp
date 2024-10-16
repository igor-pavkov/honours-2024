static void ff_layout_write_count_stats(struct rpc_task *task, void *data)
{
	struct nfs_pgio_header *hdr = data;

	ff_layout_write_record_layoutstats_done(task, hdr);
	rpc_count_iostats_metrics(task,
	    &NFS_CLIENT(hdr->inode)->cl_metrics[NFSPROC4_CLNT_WRITE]);
}