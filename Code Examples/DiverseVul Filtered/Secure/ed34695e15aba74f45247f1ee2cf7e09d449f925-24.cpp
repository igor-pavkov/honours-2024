static void ff_layout_commit_prepare_v4(struct rpc_task *task, void *data)
{
	struct nfs_commit_data *wdata = data;

	if (nfs4_setup_sequence(wdata->ds_clp,
				&wdata->args.seq_args,
				&wdata->res.seq_res,
				task))
		return;
	ff_layout_commit_prepare_common(task, data);
}