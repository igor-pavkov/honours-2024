static void handle_nttrans(connection_struct *conn,
			   struct trans_state *state,
			   struct smb_request *req)
{
	if (get_Protocol() >= PROTOCOL_NT1) {
		req->flags2 |= 0x40; /* IS_LONG_NAME */
		SSVAL(discard_const_p(uint8_t, req->inbuf),smb_flg2,req->flags2);
	}


	SMB_PERFCOUNT_SET_SUBOP(&req->pcd, state->call);

	/* Now we must call the relevant NT_TRANS function */
	switch(state->call) {
		case NT_TRANSACT_CREATE:
		{
			START_PROFILE(NT_transact_create);
			call_nt_transact_create(
				conn, req,
				&state->setup, state->setup_count,
				&state->param, state->total_param,
				&state->data, state->total_data,
				state->max_data_return);
			END_PROFILE(NT_transact_create);
			break;
		}

		case NT_TRANSACT_IOCTL:
		{
			START_PROFILE(NT_transact_ioctl);
			call_nt_transact_ioctl(
				conn, req,
				&state->setup, state->setup_count,
				&state->param, state->total_param,
				&state->data, state->total_data,
				state->max_data_return);
			END_PROFILE(NT_transact_ioctl);
			break;
		}

		case NT_TRANSACT_SET_SECURITY_DESC:
		{
			START_PROFILE(NT_transact_set_security_desc);
			call_nt_transact_set_security_desc(
				conn, req,
				&state->setup, state->setup_count,
				&state->param, state->total_param,
				&state->data, state->total_data,
				state->max_data_return);
			END_PROFILE(NT_transact_set_security_desc);
			break;
		}

		case NT_TRANSACT_NOTIFY_CHANGE:
		{
			START_PROFILE(NT_transact_notify_change);
			call_nt_transact_notify_change(
				conn, req,
				&state->setup, state->setup_count,
				&state->param, state->total_param,
				&state->data, state->total_data,
				state->max_data_return,
				state->max_param_return);
			END_PROFILE(NT_transact_notify_change);
			break;
		}

		case NT_TRANSACT_RENAME:
		{
			START_PROFILE(NT_transact_rename);
			call_nt_transact_rename(
				conn, req,
				&state->setup, state->setup_count,
				&state->param, state->total_param,
				&state->data, state->total_data,
				state->max_data_return);
			END_PROFILE(NT_transact_rename);
			break;
		}

		case NT_TRANSACT_QUERY_SECURITY_DESC:
		{
			START_PROFILE(NT_transact_query_security_desc);
			call_nt_transact_query_security_desc(
				conn, req,
				&state->setup, state->setup_count,
				&state->param, state->total_param,
				&state->data, state->total_data,
				state->max_data_return);
			END_PROFILE(NT_transact_query_security_desc);
			break;
		}

#ifdef HAVE_SYS_QUOTAS
		case NT_TRANSACT_GET_USER_QUOTA:
		{
			START_PROFILE(NT_transact_get_user_quota);
			call_nt_transact_get_user_quota(
				conn, req,
				&state->setup, state->setup_count,
				&state->param, state->total_param,
				&state->data, state->total_data,
				state->max_data_return);
			END_PROFILE(NT_transact_get_user_quota);
			break;
		}

		case NT_TRANSACT_SET_USER_QUOTA:
		{
			START_PROFILE(NT_transact_set_user_quota);
			call_nt_transact_set_user_quota(
				conn, req,
				&state->setup, state->setup_count,
				&state->param, state->total_param,
				&state->data, state->total_data,
				state->max_data_return);
			END_PROFILE(NT_transact_set_user_quota);
			break;
		}
#endif /* HAVE_SYS_QUOTAS */

		default:
			/* Error in request */
			DEBUG(0,("handle_nttrans: Unknown request %d in "
				 "nttrans call\n", state->call));
			reply_nterror(req, NT_STATUS_INVALID_LEVEL);
			return;
	}
	return;
}