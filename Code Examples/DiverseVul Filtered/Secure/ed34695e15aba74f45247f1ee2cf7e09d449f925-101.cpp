static void ff_layout_io_track_ds_error(struct pnfs_layout_segment *lseg,
					u32 idx, u64 offset, u64 length,
					u32 *op_status, int opnum, int error)
{
	struct nfs4_ff_layout_mirror *mirror;
	u32 status = *op_status;
	int err;

	if (status == 0) {
		switch (error) {
		case -ETIMEDOUT:
		case -EPFNOSUPPORT:
		case -EPROTONOSUPPORT:
		case -EOPNOTSUPP:
		case -ECONNREFUSED:
		case -ECONNRESET:
		case -EHOSTDOWN:
		case -EHOSTUNREACH:
		case -ENETUNREACH:
		case -EADDRINUSE:
		case -ENOBUFS:
		case -EPIPE:
		case -EPERM:
			*op_status = status = NFS4ERR_NXIO;
			break;
		case -EACCES:
			*op_status = status = NFS4ERR_ACCESS;
			break;
		default:
			return;
		}
	}

	mirror = FF_LAYOUT_COMP(lseg, idx);
	err = ff_layout_track_ds_error(FF_LAYOUT_FROM_HDR(lseg->pls_layout),
				       mirror, offset, length, status, opnum,
				       GFP_NOIO);

	switch (status) {
	case NFS4ERR_DELAY:
	case NFS4ERR_GRACE:
		break;
	case NFS4ERR_NXIO:
		ff_layout_mark_ds_unreachable(lseg, idx);
		/*
		 * Don't return the layout if this is a read and we still
		 * have layouts to try
		 */
		if (opnum == OP_READ)
			break;
		fallthrough;
	default:
		pnfs_error_mark_layout_for_return(lseg->pls_layout->plh_inode,
						  lseg);
	}

	dprintk("%s: err %d op %d status %u\n", __func__, err, opnum, status);
}