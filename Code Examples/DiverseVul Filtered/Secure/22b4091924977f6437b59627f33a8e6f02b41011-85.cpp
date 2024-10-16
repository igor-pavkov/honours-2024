static NTSTATUS smb_set_posix_lock(connection_struct *conn,
				struct smb_request *req,
				const char *pdata,
				int total_data,
				files_struct *fsp)
{
	struct tevent_req *subreq = NULL;
	struct smbd_lock_element *lck = NULL;
	uint64_t count;
	uint64_t offset;
	uint64_t smblctx;
	bool blocking_lock = False;
	enum brl_type lock_type;

	NTSTATUS status = NT_STATUS_OK;

	if (fsp == NULL ||
	    fsp->fsp_flags.is_pathref ||
	    fsp_get_io_fd(fsp) == -1)
	{
		return NT_STATUS_INVALID_HANDLE;
	}

	if (total_data != POSIX_LOCK_DATA_SIZE) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	switch (SVAL(pdata, POSIX_LOCK_TYPE_OFFSET)) {
		case POSIX_LOCK_TYPE_READ:
			lock_type = READ_LOCK;
			break;
		case POSIX_LOCK_TYPE_WRITE:
			/* Return the right POSIX-mappable error code for files opened read-only. */
			if (!fsp->fsp_flags.can_write) {
				return NT_STATUS_INVALID_HANDLE;
			}
			lock_type = WRITE_LOCK;
			break;
		case POSIX_LOCK_TYPE_UNLOCK:
			lock_type = UNLOCK_LOCK;
			break;
		default:
			return NT_STATUS_INVALID_PARAMETER;
	}

	switch (SVAL(pdata, POSIX_LOCK_FLAGS_OFFSET)) {
	case POSIX_LOCK_FLAG_NOWAIT:
		blocking_lock = false;
		break;
	case POSIX_LOCK_FLAG_WAIT:
		blocking_lock = true;
		break;
	default:
		return NT_STATUS_INVALID_PARAMETER;
	}

	if (!lp_blocking_locks(SNUM(conn))) { 
		blocking_lock = False;
	}

	smblctx = (uint64_t)IVAL(pdata, POSIX_LOCK_PID_OFFSET);
	offset = (((uint64_t) IVAL(pdata,(POSIX_LOCK_START_OFFSET+4))) << 32) |
			((uint64_t) IVAL(pdata,POSIX_LOCK_START_OFFSET));
	count = (((uint64_t) IVAL(pdata,(POSIX_LOCK_LEN_OFFSET+4))) << 32) |
			((uint64_t) IVAL(pdata,POSIX_LOCK_LEN_OFFSET));

	DBG_DEBUG("file %s, lock_type = %u, smblctx = %"PRIu64", "
		  "count = %"PRIu64", offset = %"PRIu64"\n",
		  fsp_str_dbg(fsp),
		  (unsigned int)lock_type,
		  smblctx,
		  count,
		  offset);

	if (lock_type == UNLOCK_LOCK) {
		struct smbd_lock_element l = {
			.req_guid = smbd_request_guid(req, 0),
			.smblctx = smblctx,
			.brltype = UNLOCK_LOCK,
			.offset = offset,
			.count = count,
		};
		status = smbd_do_unlocking(req, fsp, 1, &l, POSIX_LOCK);
		return status;
	}

	lck = talloc(req, struct smbd_lock_element);
	if (lck == NULL) {
		return NT_STATUS_NO_MEMORY;
	}

	*lck = (struct smbd_lock_element) {
		.req_guid = smbd_request_guid(req, 0),
		.smblctx = smblctx,
		.brltype = lock_type,
		.count = count,
		.offset = offset,
	};

	subreq = smbd_smb1_do_locks_send(
		fsp,
		req->sconn->ev_ctx,
		&req,
		fsp,
		blocking_lock ? UINT32_MAX : 0,
		true,		/* large_offset */
		POSIX_LOCK,
		1,
		lck);
	if (subreq == NULL) {
		TALLOC_FREE(lck);
		return NT_STATUS_NO_MEMORY;
	}
	tevent_req_set_callback(subreq, smb_set_posix_lock_done, req);
	return NT_STATUS_EVENT_PENDING;
}