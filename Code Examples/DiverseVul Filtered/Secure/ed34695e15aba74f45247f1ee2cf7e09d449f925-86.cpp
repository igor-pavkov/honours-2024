static int ff_layout_async_handle_error(struct rpc_task *task,
					struct nfs4_state *state,
					struct nfs_client *clp,
					struct pnfs_layout_segment *lseg,
					u32 idx)
{
	int vers = clp->cl_nfs_mod->rpc_vers->number;

	if (task->tk_status >= 0) {
		ff_layout_mark_ds_reachable(lseg, idx);
		return 0;
	}

	/* Handle the case of an invalid layout segment */
	if (!pnfs_is_valid_lseg(lseg))
		return -NFS4ERR_RESET_TO_PNFS;

	switch (vers) {
	case 3:
		return ff_layout_async_handle_error_v3(task, lseg, idx);
	case 4:
		return ff_layout_async_handle_error_v4(task, state, clp,
						       lseg, idx);
	default:
		/* should never happen */
		WARN_ON_ONCE(1);
		return 0;
	}
}