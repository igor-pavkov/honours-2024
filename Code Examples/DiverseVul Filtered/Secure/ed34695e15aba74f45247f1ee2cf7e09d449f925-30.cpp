static int ff_layout_initiate_commit(struct nfs_commit_data *data, int how)
{
	struct pnfs_layout_segment *lseg = data->lseg;
	struct nfs4_pnfs_ds *ds;
	struct rpc_clnt *ds_clnt;
	struct nfs4_ff_layout_mirror *mirror;
	const struct cred *ds_cred;
	u32 idx;
	int vers, ret;
	struct nfs_fh *fh;

	if (!lseg || !(pnfs_is_valid_lseg(lseg) ||
	    test_bit(NFS_LSEG_LAYOUTRETURN, &lseg->pls_flags)))
		goto out_err;

	idx = calc_ds_index_from_commit(lseg, data->ds_commit_index);
	mirror = FF_LAYOUT_COMP(lseg, idx);
	ds = nfs4_ff_layout_prepare_ds(lseg, mirror, true);
	if (!ds)
		goto out_err;

	ds_clnt = nfs4_ff_find_or_create_ds_client(mirror, ds->ds_clp,
						   data->inode);
	if (IS_ERR(ds_clnt))
		goto out_err;

	ds_cred = ff_layout_get_ds_cred(mirror, &lseg->pls_range, data->cred);
	if (!ds_cred)
		goto out_err;

	vers = nfs4_ff_layout_ds_version(mirror);

	dprintk("%s ino %lu, how %d cl_count %d vers %d\n", __func__,
		data->inode->i_ino, how, refcount_read(&ds->ds_clp->cl_count),
		vers);
	data->commit_done_cb = ff_layout_commit_done_cb;
	data->cred = ds_cred;
	refcount_inc(&ds->ds_clp->cl_count);
	data->ds_clp = ds->ds_clp;
	fh = select_ds_fh_from_commit(lseg, data->ds_commit_index);
	if (fh)
		data->args.fh = fh;

	ret = nfs_initiate_commit(ds_clnt, data, ds->ds_clp->rpc_ops,
				   vers == 3 ? &ff_layout_commit_call_ops_v3 :
					       &ff_layout_commit_call_ops_v4,
				   how, RPC_TASK_SOFTCONN);
	put_cred(ds_cred);
	return ret;
out_err:
	pnfs_generic_prepare_to_resend_writes(data);
	pnfs_generic_commit_release(data);
	return -EAGAIN;
}