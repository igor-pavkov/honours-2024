static void ff_layout_read_call_done(struct rpc_task *task, void *data)
{
	struct nfs_pgio_header *hdr = data;

	dprintk("--> %s task->tk_status %d\n", __func__, task->tk_status);

	if (test_bit(NFS_IOHDR_REDO, &hdr->flags) &&
	    task->tk_status == 0) {
		nfs4_sequence_done(task, &hdr->res.seq_res);
		return;
	}

	/* Note this may cause RPC to be resent */
	hdr->mds_ops->rpc_call_done(task, hdr);
}