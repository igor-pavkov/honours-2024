static bool delay_for_oplock(files_struct *fsp,
			     int oplock_request,
			     const struct smb2_lease *lease,
			     struct share_mode_lock *lck,
			     bool have_sharing_violation,
			     uint32_t create_disposition,
			     bool first_open_attempt)
{
	struct share_mode_data *d = lck->data;
	uint32_t i;
	bool delay = false;
	bool will_overwrite;

	if ((oplock_request & INTERNAL_OPEN_ONLY) ||
	    is_stat_open(fsp->access_mask)) {
		return false;
	}

	switch (create_disposition) {
	case FILE_SUPERSEDE:
	case FILE_OVERWRITE:
	case FILE_OVERWRITE_IF:
		will_overwrite = true;
		break;
	default:
		will_overwrite = false;
		break;
	}

	for (i=0; i<d->num_share_modes; i++) {
		struct share_mode_entry *e = &d->share_modes[i];
		struct share_mode_lease *l = NULL;
		uint32_t e_lease_type = get_lease_type(d, e);
		uint32_t break_to;
		uint32_t delay_mask = 0;

		if (e->op_type == LEASE_OPLOCK) {
			l = &d->leases[e->lease_idx];
		}

		if (have_sharing_violation) {
			delay_mask = SMB2_LEASE_HANDLE;
		} else {
			delay_mask = SMB2_LEASE_WRITE;
		}

		break_to = e_lease_type & ~delay_mask;

		if (will_overwrite) {
			/*
			 * we'll decide about SMB2_LEASE_READ later.
			 *
			 * Maybe the break will be defered
			 */
			break_to &= ~SMB2_LEASE_HANDLE;
		}

		DEBUG(10, ("entry %u: e_lease_type %u, will_overwrite: %u\n",
			   (unsigned)i, (unsigned)e_lease_type,
			   (unsigned)will_overwrite));

		if (lease != NULL && l != NULL) {
			bool ign;

			ign = smb2_lease_equal(fsp_client_guid(fsp),
					       &lease->lease_key,
					       &l->client_guid,
					       &l->lease_key);
			if (ign) {
				continue;
			}
		}

		if ((e_lease_type & ~break_to) == 0) {
			if (l != NULL && l->breaking) {
				delay = true;
			}
			continue;
		}

		if (share_mode_stale_pid(d, i)) {
			continue;
		}

		if (will_overwrite) {
			/*
			 * If we break anyway break to NONE directly.
			 * Otherwise vfs_set_filelen() will trigger the
			 * break.
			 */
			break_to &= ~(SMB2_LEASE_READ|SMB2_LEASE_WRITE);
		}

		if (e->op_type != LEASE_OPLOCK) {
			/*
			 * Oplocks only support breaking to R or NONE.
			 */
			break_to &= ~(SMB2_LEASE_HANDLE|SMB2_LEASE_WRITE);
		}

		DEBUG(10, ("breaking from %d to %d\n",
			   (int)e_lease_type, (int)break_to));
		send_break_message(fsp->conn->sconn->msg_ctx, e,
				   break_to);
		if (e_lease_type & delay_mask) {
			delay = true;
		}
		if (l != NULL && l->breaking && !first_open_attempt) {
			delay = true;
		}
		continue;
	}

	return delay;
}