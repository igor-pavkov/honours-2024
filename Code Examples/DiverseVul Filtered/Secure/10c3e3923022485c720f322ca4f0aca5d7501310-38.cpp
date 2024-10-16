static NTSTATUS grant_fsp_oplock_type(struct smb_request *req,
				      struct files_struct *fsp,
				      struct share_mode_lock *lck,
				      int oplock_request,
				      struct smb2_lease *lease)
{
	struct share_mode_data *d = lck->data;
	bool got_handle_lease = false;
	bool got_oplock = false;
	uint32_t i;
	uint32_t granted;
	uint32_t lease_idx = UINT32_MAX;
	bool ok;
	NTSTATUS status;

	if (oplock_request & INTERNAL_OPEN_ONLY) {
		/* No oplocks on internal open. */
		oplock_request = NO_OPLOCK;
		DEBUG(10,("grant_fsp_oplock_type: oplock type 0x%x on file %s\n",
			fsp->oplock_type, fsp_str_dbg(fsp)));
	}

	if (oplock_request == LEASE_OPLOCK) {
		if (lease == NULL) {
			/*
			 * The SMB2 layer should have checked this
			 */
			return NT_STATUS_INTERNAL_ERROR;
		}

		granted = lease->lease_state;

		if (lp_kernel_oplocks(SNUM(fsp->conn))) {
			DEBUG(10, ("No lease granted because kernel oplocks are enabled\n"));
			granted = SMB2_LEASE_NONE;
		}
		if ((granted & (SMB2_LEASE_READ|SMB2_LEASE_WRITE)) == 0) {
			DEBUG(10, ("No read or write lease requested\n"));
			granted = SMB2_LEASE_NONE;
		}
		if (granted == SMB2_LEASE_WRITE) {
			DEBUG(10, ("pure write lease requested\n"));
			granted = SMB2_LEASE_NONE;
		}
		if (granted == (SMB2_LEASE_WRITE|SMB2_LEASE_HANDLE)) {
			DEBUG(10, ("write and handle lease requested\n"));
			granted = SMB2_LEASE_NONE;
		}
	} else {
		granted = map_oplock_to_lease_type(
			oplock_request & ~SAMBA_PRIVATE_OPLOCK_MASK);
	}

	if (lp_locking(fsp->conn->params) && file_has_brlocks(fsp)) {
		DEBUG(10,("grant_fsp_oplock_type: file %s has byte range locks\n",
			fsp_str_dbg(fsp)));
		granted &= ~SMB2_LEASE_READ;
	}

	for (i=0; i<d->num_share_modes; i++) {
		struct share_mode_entry *e = &d->share_modes[i];
		uint32_t e_lease_type;

		e_lease_type = get_lease_type(d, e);

		if ((granted & SMB2_LEASE_WRITE) &&
		    !is_same_lease(fsp, d, e, lease) &&
		    !share_mode_stale_pid(d, i)) {
			/*
			 * Can grant only one writer
			 */
			granted &= ~SMB2_LEASE_WRITE;
		}

		if ((e_lease_type & SMB2_LEASE_HANDLE) && !got_handle_lease &&
		    !share_mode_stale_pid(d, i)) {
			got_handle_lease = true;
		}

		if ((e->op_type != LEASE_OPLOCK) && !got_oplock &&
		    !share_mode_stale_pid(d, i)) {
			got_oplock = true;
		}
	}

	if ((granted & SMB2_LEASE_READ) && !(granted & SMB2_LEASE_WRITE)) {
		bool allow_level2 =
			(global_client_caps & CAP_LEVEL_II_OPLOCKS) &&
			lp_level2_oplocks(SNUM(fsp->conn));

		if (!allow_level2) {
			granted = SMB2_LEASE_NONE;
		}
	}

	if (oplock_request == LEASE_OPLOCK) {
		if (got_oplock) {
			granted &= ~SMB2_LEASE_HANDLE;
		}

		fsp->oplock_type = LEASE_OPLOCK;

		status = grant_fsp_lease(fsp, lck, lease, &lease_idx,
					 granted);
		if (!NT_STATUS_IS_OK(status)) {
			return status;

		}
		*lease = fsp->lease->lease;
		DEBUG(10, ("lease_state=%d\n", lease->lease_state));
	} else {
		if (got_handle_lease) {
			granted = SMB2_LEASE_NONE;
		}

		switch (granted) {
		case SMB2_LEASE_READ|SMB2_LEASE_WRITE|SMB2_LEASE_HANDLE:
			fsp->oplock_type = BATCH_OPLOCK|EXCLUSIVE_OPLOCK;
			break;
		case SMB2_LEASE_READ|SMB2_LEASE_WRITE:
			fsp->oplock_type = EXCLUSIVE_OPLOCK;
			break;
		case SMB2_LEASE_READ|SMB2_LEASE_HANDLE:
		case SMB2_LEASE_READ:
			fsp->oplock_type = LEVEL_II_OPLOCK;
			break;
		default:
			fsp->oplock_type = NO_OPLOCK;
			break;
		}

		status = set_file_oplock(fsp);
		if (!NT_STATUS_IS_OK(status)) {
			/*
			 * Could not get the kernel oplock
			 */
			fsp->oplock_type = NO_OPLOCK;
		}
	}

	ok = set_share_mode(lck, fsp, get_current_uid(fsp->conn),
			    req ? req->mid : 0,
			    fsp->oplock_type,
			    lease_idx);
	if (!ok) {
		return NT_STATUS_NO_MEMORY;
	}

	ok = update_num_read_oplocks(fsp, lck);
	if (!ok) {
		del_share_mode(lck, fsp);
		return NT_STATUS_INTERNAL_ERROR;
	}

	DEBUG(10,("grant_fsp_oplock_type: oplock type 0x%x on file %s\n",
		  fsp->oplock_type, fsp_str_dbg(fsp)));

	return NT_STATUS_OK;
}