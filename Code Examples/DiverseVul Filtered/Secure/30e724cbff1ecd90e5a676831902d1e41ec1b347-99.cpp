static NTSTATUS vfswrap_get_dfs_referrals(struct vfs_handle_struct *handle,
					  struct dfs_GetDFSReferral *r)
{
	struct junction_map *junction = NULL;
	int consumedcnt = 0;
	bool self_referral = false;
	char *pathnamep = NULL;
	char *local_dfs_path = NULL;
	NTSTATUS status;
	int i;
	uint16_t max_referral_level = r->in.req.max_referral_level;

	if (DEBUGLVL(10)) {
		NDR_PRINT_IN_DEBUG(dfs_GetDFSReferral, r);
	}

	/* get the junction entry */
	if (r->in.req.servername == NULL) {
		return NT_STATUS_NOT_FOUND;
	}

	/*
	 * Trim pathname sent by client so it begins with only one backslash.
	 * Two backslashes confuse some dfs clients
	 */

	local_dfs_path = talloc_strdup(r, r->in.req.servername);
	if (local_dfs_path == NULL) {
		return NT_STATUS_NO_MEMORY;
	}
	pathnamep = local_dfs_path;
	while (IS_DIRECTORY_SEP(pathnamep[0]) &&
	       IS_DIRECTORY_SEP(pathnamep[1])) {
		pathnamep++;
	}

	junction = talloc_zero(r, struct junction_map);
	if (junction == NULL) {
		return NT_STATUS_NO_MEMORY;
	}

	/* The following call can change cwd. */
	status = get_referred_path(r, pathnamep,
				   !handle->conn->sconn->using_smb2,
				   junction, &consumedcnt, &self_referral);
	if (!NT_STATUS_IS_OK(status)) {
		vfs_ChDir(handle->conn, handle->conn->connectpath);
		return status;
	}
	vfs_ChDir(handle->conn, handle->conn->connectpath);

	if (!self_referral) {
		pathnamep[consumedcnt] = '\0';

		if (DEBUGLVL(3)) {
			dbgtext("setup_dfs_referral: Path %s to "
				"alternate path(s):",
				pathnamep);
			for (i=0; i < junction->referral_count; i++) {
				dbgtext(" %s",
				junction->referral_list[i].alternate_path);
			}
			dbgtext(".\n");
		}
	}

	if (r->in.req.max_referral_level <= 2) {
		max_referral_level = 2;
	}
	if (r->in.req.max_referral_level >= 3) {
		max_referral_level = 3;
	}

	r->out.resp = talloc_zero(r, struct dfs_referral_resp);
	if (r->out.resp == NULL) {
		return NT_STATUS_NO_MEMORY;
	}

	r->out.resp->path_consumed = strlen_m(pathnamep) * 2;
	r->out.resp->nb_referrals = junction->referral_count;

	r->out.resp->header_flags = DFS_HEADER_FLAG_STORAGE_SVR;
	if (self_referral) {
		r->out.resp->header_flags |= DFS_HEADER_FLAG_REFERAL_SVR;
	}

	r->out.resp->referral_entries = talloc_zero_array(r,
				struct dfs_referral_type,
				r->out.resp->nb_referrals);
	if (r->out.resp->referral_entries == NULL) {
		return NT_STATUS_NO_MEMORY;
	}

	switch (max_referral_level) {
	case 2:
		for(i=0; i < junction->referral_count; i++) {
			struct referral *ref = &junction->referral_list[i];
			TALLOC_CTX *mem_ctx = r->out.resp->referral_entries;
			struct dfs_referral_type *t =
				&r->out.resp->referral_entries[i];
			struct dfs_referral_v2 *v2 = &t->referral.v2;

			t->version = 2;
			v2->size = VERSION2_REFERRAL_SIZE;
			if (self_referral) {
				v2->server_type = DFS_SERVER_ROOT;
			} else {
				v2->server_type = DFS_SERVER_NON_ROOT;
			}
			v2->entry_flags = 0;
			v2->proximity = ref->proximity;
			v2->ttl = ref->ttl;
			v2->DFS_path = talloc_strdup(mem_ctx, pathnamep);
			if (v2->DFS_path == NULL) {
				return NT_STATUS_NO_MEMORY;
			}
			v2->DFS_alt_path = talloc_strdup(mem_ctx, pathnamep);
			if (v2->DFS_alt_path == NULL) {
				return NT_STATUS_NO_MEMORY;
			}
			v2->netw_address = talloc_strdup(mem_ctx,
							 ref->alternate_path);
			if (v2->netw_address == NULL) {
				return NT_STATUS_NO_MEMORY;
			}
		}

		break;
	case 3:
		for(i=0; i < junction->referral_count; i++) {
			struct referral *ref = &junction->referral_list[i];
			TALLOC_CTX *mem_ctx = r->out.resp->referral_entries;
			struct dfs_referral_type *t =
				&r->out.resp->referral_entries[i];
			struct dfs_referral_v3 *v3 = &t->referral.v3;
			struct dfs_normal_referral *r1 = &v3->referrals.r1;

			t->version = 3;
			v3->size = VERSION3_REFERRAL_SIZE;
			if (self_referral) {
				v3->server_type = DFS_SERVER_ROOT;
			} else {
				v3->server_type = DFS_SERVER_NON_ROOT;
			}
			v3->entry_flags = 0;
			v3->ttl = ref->ttl;
			r1->DFS_path = talloc_strdup(mem_ctx, pathnamep);
			if (r1->DFS_path == NULL) {
				return NT_STATUS_NO_MEMORY;
			}
			r1->DFS_alt_path = talloc_strdup(mem_ctx, pathnamep);
			if (r1->DFS_alt_path == NULL) {
				return NT_STATUS_NO_MEMORY;
			}
			r1->netw_address = talloc_strdup(mem_ctx,
							 ref->alternate_path);
			if (r1->netw_address == NULL) {
				return NT_STATUS_NO_MEMORY;
			}
		}
		break;
	default:
		DEBUG(0,("setup_dfs_referral: Invalid dfs referral "
			"version: %d\n",
			max_referral_level));
		return NT_STATUS_INVALID_LEVEL;
	}

	if (DEBUGLVL(10)) {
		NDR_PRINT_OUT_DEBUG(dfs_GetDFSReferral, r);
	}

	return NT_STATUS_OK;
}