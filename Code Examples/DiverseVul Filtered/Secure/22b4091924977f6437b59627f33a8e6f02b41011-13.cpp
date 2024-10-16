void reply_trans2(struct smb_request *req)
{
	connection_struct *conn = req->conn;
	unsigned int dsoff;
	unsigned int dscnt;
	unsigned int psoff;
	unsigned int pscnt;
	unsigned int tran_call;
	struct trans_state *state;
	NTSTATUS result;

	START_PROFILE(SMBtrans2);

	if (req->wct < 14) {
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		END_PROFILE(SMBtrans2);
		return;
	}

	dsoff = SVAL(req->vwv+12, 0);
	dscnt = SVAL(req->vwv+11, 0);
	psoff = SVAL(req->vwv+10, 0);
	pscnt = SVAL(req->vwv+9, 0);
	tran_call = SVAL(req->vwv+14, 0);

	result = allow_new_trans(conn->pending_trans, req->mid);
	if (!NT_STATUS_IS_OK(result)) {
		DEBUG(2, ("Got invalid trans2 request: %s\n",
			  nt_errstr(result)));
		reply_nterror(req, result);
		END_PROFILE(SMBtrans2);
		return;
	}

	if (IS_IPC(conn)) {
		switch (tran_call) {
		/* List the allowed trans2 calls on IPC$ */
		case TRANSACT2_OPEN:
		case TRANSACT2_GET_DFS_REFERRAL:
		case TRANSACT2_QFILEINFO:
		case TRANSACT2_QFSINFO:
		case TRANSACT2_SETFSINFO:
			break;
		default:
			reply_nterror(req, NT_STATUS_ACCESS_DENIED);
			END_PROFILE(SMBtrans2);
			return;
		}
	}

	if ((state = talloc(conn, struct trans_state)) == NULL) {
		DEBUG(0, ("talloc failed\n"));
		reply_nterror(req, NT_STATUS_NO_MEMORY);
		END_PROFILE(SMBtrans2);
		return;
	}

	state->cmd = SMBtrans2;

	state->mid = req->mid;
	state->vuid = req->vuid;
	state->setup_count = SVAL(req->vwv+13, 0);
	state->setup = NULL;
	state->total_param = SVAL(req->vwv+0, 0);
	state->param = NULL;
	state->total_data =  SVAL(req->vwv+1, 0);
	state->data = NULL;
	state->max_param_return = SVAL(req->vwv+2, 0);
	state->max_data_return  = SVAL(req->vwv+3, 0);
	state->max_setup_return = SVAL(req->vwv+4, 0);
	state->close_on_completion = BITSETW(req->vwv+5, 0);
	state->one_way = BITSETW(req->vwv+5, 1);

	state->call = tran_call;

	/* All trans2 messages we handle have smb_sucnt == 1 - ensure this
	   is so as a sanity check */
	if (state->setup_count != 1) {
		/*
		 * Need to have rc=0 for ioctl to get job id for OS/2.
		 *  Network printing will fail if function is not successful.
		 *  Similar function in reply.c will be used if protocol
		 *  is LANMAN1.0 instead of LM1.2X002.
		 *  Until DosPrintSetJobInfo with PRJINFO3 is supported,
		 *  outbuf doesn't have to be set(only job id is used).
		 */
		if ( (state->setup_count == 4)
		     && (tran_call == TRANSACT2_IOCTL)
		     && (SVAL(req->vwv+16, 0) == LMCAT_SPL)
		     &&	(SVAL(req->vwv+17, 0) == LMFUNC_GETJOBID)) {
			DEBUG(2,("Got Trans2 DevIOctl jobid\n"));
		} else {
			DEBUG(2,("Invalid smb_sucnt in trans2 call(%u)\n",state->setup_count));
			DEBUG(2,("Transaction is %d\n",tran_call));
			TALLOC_FREE(state);
			reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
			END_PROFILE(SMBtrans2);
			return;
		}
	}

	if ((dscnt > state->total_data) || (pscnt > state->total_param))
		goto bad_param;

	if (state->total_data) {

		if (trans_oob(state->total_data, 0, dscnt)
		    || trans_oob(smb_len(req->inbuf), dsoff, dscnt)) {
			goto bad_param;
		}

		/* Can't use talloc here, the core routines do realloc on the
		 * params and data. */
		state->data = (char *)SMB_MALLOC(state->total_data);
		if (state->data == NULL) {
			DEBUG(0,("reply_trans2: data malloc fail for %u "
				 "bytes !\n", (unsigned int)state->total_data));
			TALLOC_FREE(state);
			reply_nterror(req, NT_STATUS_NO_MEMORY);
			END_PROFILE(SMBtrans2);
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
		state->param = (char *)SMB_MALLOC(state->total_param);
		if (state->param == NULL) {
			DEBUG(0,("reply_trans: param malloc fail for %u "
				 "bytes !\n", (unsigned int)state->total_param));
			SAFE_FREE(state->data);
			TALLOC_FREE(state);
			reply_nterror(req, NT_STATUS_NO_MEMORY);
			END_PROFILE(SMBtrans2);
			return;
		} 

		memcpy(state->param,smb_base(req->inbuf)+psoff,pscnt);
	}

	state->received_data  = dscnt;
	state->received_param = pscnt;

	if ((state->received_param == state->total_param) &&
	    (state->received_data == state->total_data)) {

		handle_trans2(conn, req, state);

		SAFE_FREE(state->data);
		SAFE_FREE(state->param);
		TALLOC_FREE(state);
		END_PROFILE(SMBtrans2);
		return;
	}

	DLIST_ADD(conn->pending_trans, state);

	/* We need to send an interim response then receive the rest
	   of the parameter/data bytes */
	reply_outbuf(req, 0, 0);
	show_msg((char *)req->outbuf);
	END_PROFILE(SMBtrans2);
	return;

  bad_param:

	DEBUG(0,("reply_trans2: invalid trans parameters\n"));
	SAFE_FREE(state->data);
	SAFE_FREE(state->param);
	TALLOC_FREE(state);
	END_PROFILE(SMBtrans2);
	reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
}