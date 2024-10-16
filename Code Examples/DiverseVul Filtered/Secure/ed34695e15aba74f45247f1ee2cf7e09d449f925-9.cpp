static int ff_layout_async_handle_error_v3(struct rpc_task *task,
					   struct pnfs_layout_segment *lseg,
					   u32 idx)
{
	struct nfs4_deviceid_node *devid = FF_LAYOUT_DEVID_NODE(lseg, idx);

	switch (task->tk_status) {
	/* File access problems. Don't mark the device as unavailable */
	case -EACCES:
	case -ESTALE:
	case -EISDIR:
	case -EBADHANDLE:
	case -ELOOP:
	case -ENOSPC:
		break;
	case -EJUKEBOX:
		nfs_inc_stats(lseg->pls_layout->plh_inode, NFSIOS_DELAY);
		goto out_retry;
	default:
		dprintk("%s DS connection error %d\n", __func__,
			task->tk_status);
		nfs4_delete_deviceid(devid->ld, devid->nfs_client,
				&devid->deviceid);
	}
	/* FIXME: Need to prevent infinite looping here. */
	return -NFS4ERR_RESET_TO_PNFS;
out_retry:
	task->tk_status = 0;
	rpc_restart_call_prepare(task);
	rpc_delay(task, NFS_JUKEBOX_RETRY_TIME);
	return -EAGAIN;
}