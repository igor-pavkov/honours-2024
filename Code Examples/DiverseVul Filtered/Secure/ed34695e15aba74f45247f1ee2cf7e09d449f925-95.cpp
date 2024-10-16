static void ff_layout_write_prepare_v3(struct rpc_task *task, void *data)
{
	struct nfs_pgio_header *hdr = data;

	if (ff_layout_write_prepare_common(task, hdr))
		return;

	rpc_call_start(task);
}