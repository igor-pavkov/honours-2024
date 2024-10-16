static void ff_layout_write_prepare_v4(struct rpc_task *task, void *data)
{
	struct nfs_pgio_header *hdr = data;

	if (nfs4_setup_sequence(hdr->ds_clp,
				&hdr->args.seq_args,
				&hdr->res.seq_res,
				task))
		return;

	ff_layout_write_prepare_common(task, hdr);
}