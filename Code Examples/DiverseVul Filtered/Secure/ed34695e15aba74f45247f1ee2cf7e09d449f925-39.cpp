static int ff_layout_read_prepare_common(struct rpc_task *task,
					 struct nfs_pgio_header *hdr)
{
	if (unlikely(test_bit(NFS_CONTEXT_BAD, &hdr->args.context->flags))) {
		rpc_exit(task, -EIO);
		return -EIO;
	}

	ff_layout_read_record_layoutstats_start(task, hdr);
	return 0;
}