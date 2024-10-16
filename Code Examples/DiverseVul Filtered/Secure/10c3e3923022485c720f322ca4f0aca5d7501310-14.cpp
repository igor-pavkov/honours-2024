static NTSTATUS lease_match(connection_struct *conn,
			    struct smb_request *req,
			    struct smb2_lease_key *lease_key,
			    const char *servicepath,
			    const struct smb_filename *fname,
			    uint16_t *p_version,
			    uint16_t *p_epoch)
{
	struct smbd_server_connection *sconn = req->sconn;
	TALLOC_CTX *tos = talloc_tos();
	struct lease_match_state state = {
		.mem_ctx = tos,
		.servicepath = servicepath,
		.fname = fname,
		.match_status = NT_STATUS_OK
	};
	uint32_t i;
	NTSTATUS status;

	state.file_existed = VALID_STAT(fname->st);
	if (state.file_existed) {
		state.id = vfs_file_id_from_sbuf(conn, &fname->st);
	} else {
		memset(&state.id, '\0', sizeof(state.id));
	}

	status = leases_db_parse(&sconn->client->connections->smb2.client.guid,
				 lease_key, lease_match_parser, &state);
	if (!NT_STATUS_IS_OK(status)) {
		/*
		 * Not found or error means okay: We can make the lease pass
		 */
		return NT_STATUS_OK;
	}
	if (!NT_STATUS_EQUAL(state.match_status, NT_STATUS_OPLOCK_NOT_GRANTED)) {
		/*
		 * Anything but NT_STATUS_OPLOCK_NOT_GRANTED, let the caller
		 * deal with it.
		 */
		return state.match_status;
	}

	/* We have to break all existing leases. */
	for (i = 0; i < state.num_file_ids; i++) {
		struct share_mode_lock *lck;
		struct share_mode_data *d;
		uint32_t j;

		if (file_id_equal(&state.ids[i], &state.id)) {
			/* Don't need to break our own file. */
			continue;
		}

		lck = get_existing_share_mode_lock(talloc_tos(), state.ids[i]);
		if (lck == NULL) {
			/* Race condition - file already closed. */
			continue;
		}
		d = lck->data;
		for (j=0; j<d->num_share_modes; j++) {
			struct share_mode_entry *e = &d->share_modes[j];
			uint32_t e_lease_type = get_lease_type(d, e);
			struct share_mode_lease *l = NULL;

			if (share_mode_stale_pid(d, j)) {
				continue;
			}

			if (e->op_type == LEASE_OPLOCK) {
				l = &lck->data->leases[e->lease_idx];
				if (!smb2_lease_key_equal(&l->lease_key,
							  lease_key)) {
					continue;
				}
				*p_epoch = l->epoch;
				*p_version = l->lease_version;
			}

			if (e_lease_type == SMB2_LEASE_NONE) {
				continue;
			}

			send_break_message(conn->sconn->msg_ctx, e,
					   SMB2_LEASE_NONE);

			/*
			 * Windows 7 and 8 lease clients
			 * are broken in that they will not
			 * respond to lease break requests
			 * whilst waiting for an outstanding
			 * open request on that lease handle
			 * on the same TCP connection, due
			 * to holding an internal inode lock.
			 *
			 * This means we can't reschedule
			 * ourselves here, but must return
			 * from the create.
			 *
			 * Work around:
			 *
			 * Send the breaks and then return
			 * SMB2_LEASE_NONE in the lease handle
			 * to cause them to acknowledge the
			 * lease break. Consulatation with
			 * Microsoft engineering confirmed
			 * this approach is safe.
			 */

		}
		TALLOC_FREE(lck);
	}
	/*
	 * Ensure we don't grant anything more so we
	 * never upgrade.
	 */
	return NT_STATUS_OPLOCK_NOT_GRANTED;
}