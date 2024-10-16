void reply_nttrans(struct smb_request *req)
{
	connection_struct *conn = req->conn;
	uint32_t pscnt;
	uint32_t psoff;
	uint32_t dscnt;
	uint32_t dsoff;
	uint16 function_code;
	NTSTATUS result;
	struct trans_state *state;

	START_PROFILE(SMBnttrans);

	if (req->wct < 19) {
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		END_PROFILE(SMBnttrans);
		return;
	}

	pscnt = IVAL(req->vwv+9, 1);
	psoff = IVAL(req->vwv+11, 1);
	dscnt = IVAL(req->vwv+13, 1);
	dsoff = IVAL(req->vwv+15, 1);
	function_code = SVAL(req->vwv+18, 0);

	if (IS_IPC(conn) && (function_code != NT_TRANSACT_CREATE)) {
		reply_nterror(req, NT_STATUS_ACCESS_DENIED);
		END_PROFILE(SMBnttrans);
		return;
	}

	result = allow_new_trans(conn->pending_trans, req->mid);
	if (!NT_STATUS_IS_OK(result)) {
		DEBUG(2, ("Got invalid nttrans request: %s\n", nt_errstr(result)));
		reply_nterror(req, result);
		END_PROFILE(SMBnttrans);
		return;
	}

	if ((state = talloc(conn, struct trans_state)) == NULL) {
		reply_nterror(req, NT_STATUS_NO_MEMORY);
		END_PROFILE(SMBnttrans);
		return;
	}

	state->cmd = SMBnttrans;

	state->mid = req->mid;
	state->vuid = req->vuid;
	state->total_data = IVAL(req->vwv+3, 1);
	state->data = NULL;
	state->total_param = IVAL(req->vwv+1, 1);
	state->param = NULL;
	state->max_data_return = IVAL(req->vwv+7, 1);
	state->max_param_return = IVAL(req->vwv+5, 1);

	/* setup count is in *words* */
	state->setup_count = 2*CVAL(req->vwv+17, 1);
	state->setup = NULL;
	state->call = function_code;

	DEBUG(10, ("num_setup=%u, "
		   "param_total=%u, this_param=%u, max_param=%u, "
		   "data_total=%u, this_data=%u, max_data=%u, "
		   "param_offset=%u, data_offset=%u\n",
		   (unsigned)state->setup_count,
		   (unsigned)state->total_param, (unsigned)pscnt,
		   (unsigned)state->max_param_return,
		   (unsigned)state->total_data, (unsigned)dscnt,
		   (unsigned)state->max_data_return,
		   (unsigned)psoff, (unsigned)dsoff));

	/*
	 * All nttrans messages we handle have smb_wct == 19 +
	 * state->setup_count.  Ensure this is so as a sanity check.
	 */

	if(req->wct != 19 + (state->setup_count/2)) {
		DEBUG(2,("Invalid smb_wct %d in nttrans call (should be %d)\n",
			 req->wct, 19 + (state->setup_count/2)));
		goto bad_param;
	}

	/* Don't allow more than 128mb for each value. */
	if ((state->total_data > (1024*1024*128)) ||
	    (state->total_param > (1024*1024*128))) {
		reply_nterror(req, NT_STATUS_NO_MEMORY);
		END_PROFILE(SMBnttrans);
		return;
	}

	if ((dscnt > state->total_data) || (pscnt > state->total_param))
		goto bad_param;

	if (state->total_data)  {

		if (trans_oob(state->total_data, 0, dscnt)
		    || trans_oob(smb_len(req->inbuf), dsoff, dscnt)) {
			goto bad_param;
		}

		/* Can't use talloc here, the core routines do realloc on the
		 * params and data. */
		if ((state->data = (char *)SMB_MALLOC(state->total_data)) == NULL) {
			DEBUG(0,("reply_nttrans: data malloc fail for %u "
				 "bytes !\n", (unsigned int)state->total_data));
			TALLOC_FREE(state);
			reply_nterror(req, NT_STATUS_NO_MEMORY);
			END_PROFILE(SMBnttrans);
			return;
		}

		memcpy(state->data,smb_base(req->inbuf)+dsoff,dscnt);
	}

	if (state->total_param) {

		if (trans_oob(state->total_param, 0, pscnt)
		    || trans_oob(smb_len(req->inbuf), psoff, pscnt)) {
			goto bad_param;
		}

		/* Can't use talloc here, the core routines do realloc on the
		 * params and data. */
		if ((state->param = (char *)SMB_MALLOC(state->total_param)) == NULL) {
			DEBUG(0,("reply_nttrans: param malloc fail for %u "
				 "bytes !\n", (unsigned int)state->total_param));
			SAFE_FREE(state->data);
			TALLOC_FREE(state);
			reply_nterror(req, NT_STATUS_NO_MEMORY);
			END_PROFILE(SMBnttrans);
			return;
		}

		memcpy(state->param,smb_base(req->inbuf)+psoff,pscnt);
	}

	state->received_data  = dscnt;
	state->received_param = pscnt;

	if(state->setup_count > 0) {
		DEBUG(10,("reply_nttrans: state->setup_count = %d\n",
			  state->setup_count));

		/*
		 * No overflow possible here, state->setup_count is an
		 * unsigned int, being filled by a single byte from
		 * CVAL(req->vwv+13, 0) above. The cast in the comparison
		 * below is not necessary, it's here to clarify things. The
		 * validity of req->vwv and req->wct has been checked in
		 * init_smb_request already.
		 */
		if ((state->setup_count/2) + 19 > (unsigned int)req->wct) {
			goto bad_param;
		}

		state->setup = (uint16 *)TALLOC(state, state->setup_count);
		if (state->setup == NULL) {
			DEBUG(0,("reply_nttrans : Out of memory\n"));
			SAFE_FREE(state->data);
			SAFE_FREE(state->param);
			TALLOC_FREE(state);
			reply_nterror(req, NT_STATUS_NO_MEMORY);
			END_PROFILE(SMBnttrans);
			return;
		}

		memcpy(state->setup, req->vwv+19, state->setup_count);
		dump_data(10, (uint8 *)state->setup, state->setup_count);
	}

	if ((state->received_data == state->total_data) &&
	    (state->received_param == state->total_param)) {
		handle_nttrans(conn, state, req);
		SAFE_FREE(state->param);
		SAFE_FREE(state->data);
		TALLOC_FREE(state);
		END_PROFILE(SMBnttrans);
		return;
	}

	DLIST_ADD(conn->pending_trans, state);

	/* We need to send an interim response then receive the rest
	   of the parameter/data bytes */
	reply_outbuf(req, 0, 0);
	show_msg((char *)req->outbuf);
	END_PROFILE(SMBnttrans);
	return;

  bad_param:

	DEBUG(0,("reply_nttrans: invalid trans parameters\n"));
	SAFE_FREE(state->data);
	SAFE_FREE(state->param);
	TALLOC_FREE(state);
	reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
	END_PROFILE(SMBnttrans);
	return;
}