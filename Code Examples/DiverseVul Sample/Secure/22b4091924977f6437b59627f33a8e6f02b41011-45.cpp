static void handle_trans2(connection_struct *conn, struct smb_request *req,
			  struct trans_state *state)
{
	if (get_Protocol() >= PROTOCOL_NT1) {
		req->flags2 |= 0x40; /* IS_LONG_NAME */
		SSVAL((discard_const_p(uint8_t, req->inbuf)),smb_flg2,req->flags2);
	}

	if (ENCRYPTION_REQUIRED(conn) && !req->encrypted) {
		if (state->call != TRANSACT2_QFSINFO &&
		    state->call != TRANSACT2_SETFSINFO) {
			DEBUG(0,("handle_trans2: encryption required "
				"with call 0x%x\n",
				(unsigned int)state->call));
			reply_nterror(req, NT_STATUS_ACCESS_DENIED);
			return;
		}
	}

	SMB_PERFCOUNT_SET_SUBOP(&req->pcd, state->call);

	/* Now we must call the relevant TRANS2 function */
	switch(state->call)  {
	case TRANSACT2_OPEN:
	{
		START_PROFILE(Trans2_open);
		call_trans2open(conn, req,
				&state->param, state->total_param,
				&state->data, state->total_data,
				state->max_data_return);
		END_PROFILE(Trans2_open);
		break;
	}

	case TRANSACT2_FINDFIRST:
	{
		START_PROFILE(Trans2_findfirst);
		call_trans2findfirst(conn, req,
				     &state->param, state->total_param,
				     &state->data, state->total_data,
				     state->max_data_return);
		END_PROFILE(Trans2_findfirst);
		break;
	}

	case TRANSACT2_FINDNEXT:
	{
		START_PROFILE(Trans2_findnext);
		call_trans2findnext(conn, req,
				    &state->param, state->total_param,
				    &state->data, state->total_data,
				    state->max_data_return);
		END_PROFILE(Trans2_findnext);
		break;
	}

	case TRANSACT2_QFSINFO:
	{
		START_PROFILE(Trans2_qfsinfo);
		call_trans2qfsinfo(conn, req,
				   &state->param, state->total_param,
				   &state->data, state->total_data,
				   state->max_data_return);
		END_PROFILE(Trans2_qfsinfo);
	    break;
	}

	case TRANSACT2_SETFSINFO:
	{
		START_PROFILE(Trans2_setfsinfo);
		call_trans2setfsinfo(conn, req,
				     &state->param, state->total_param,
				     &state->data, state->total_data,
				     state->max_data_return);
		END_PROFILE(Trans2_setfsinfo);
		break;
	}

	case TRANSACT2_QPATHINFO:
	case TRANSACT2_QFILEINFO:
	{
		START_PROFILE(Trans2_qpathinfo);
		call_trans2qfilepathinfo(conn, req, state->call,
					 &state->param, state->total_param,
					 &state->data, state->total_data,
					 state->max_data_return);
		END_PROFILE(Trans2_qpathinfo);
		break;
	}

	case TRANSACT2_SETPATHINFO:
	case TRANSACT2_SETFILEINFO:
	{
		START_PROFILE(Trans2_setpathinfo);
		call_trans2setfilepathinfo(conn, req, state->call,
					   &state->param, state->total_param,
					   &state->data, state->total_data,
					   state->max_data_return);
		END_PROFILE(Trans2_setpathinfo);
		break;
	}

	case TRANSACT2_FINDNOTIFYFIRST:
	{
		START_PROFILE(Trans2_findnotifyfirst);
		call_trans2findnotifyfirst(conn, req,
					   &state->param, state->total_param,
					   &state->data, state->total_data,
					   state->max_data_return);
		END_PROFILE(Trans2_findnotifyfirst);
		break;
	}

	case TRANSACT2_FINDNOTIFYNEXT:
	{
		START_PROFILE(Trans2_findnotifynext);
		call_trans2findnotifynext(conn, req,
					  &state->param, state->total_param,
					  &state->data, state->total_data,
					  state->max_data_return);
		END_PROFILE(Trans2_findnotifynext);
		break;
	}

	case TRANSACT2_MKDIR:
	{
		START_PROFILE(Trans2_mkdir);
		call_trans2mkdir(conn, req,
				 &state->param, state->total_param,
				 &state->data, state->total_data,
				 state->max_data_return);
		END_PROFILE(Trans2_mkdir);
		break;
	}

	case TRANSACT2_GET_DFS_REFERRAL:
	{
		START_PROFILE(Trans2_get_dfs_referral);
		call_trans2getdfsreferral(conn, req,
					  &state->param, state->total_param,
					  &state->data, state->total_data,
					  state->max_data_return);
		END_PROFILE(Trans2_get_dfs_referral);
		break;
	}

	case TRANSACT2_IOCTL:
	{
		START_PROFILE(Trans2_ioctl);
		call_trans2ioctl(conn, req,
				 &state->param, state->total_param,
				 &state->data, state->total_data,
				 state->max_data_return);
		END_PROFILE(Trans2_ioctl);
		break;
	}

	default:
		/* Error in request */
		DEBUG(2,("Unknown request %d in trans2 call\n", state->call));
		reply_nterror(req, NT_STATUS_NOT_IMPLEMENTED);
	}
}