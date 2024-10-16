void reply_transs2(struct smb_request *req)
{
	connection_struct *conn = req->conn;
	unsigned int pcnt,poff,dcnt,doff,pdisp,ddisp;
	struct trans_state *state;

	START_PROFILE(SMBtranss2);

	show_msg((const char *)req->inbuf);

	/* Windows clients expect all replies to
	   a transact secondary (SMBtranss2 0x33)
	   to have a command code of transact
	   (SMBtrans2 0x32). See bug #8989
	   and also [MS-CIFS] section 2.2.4.47.2
	   for details.
	*/
	req->cmd = SMBtrans2;

	if (req->wct < 8) {
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		END_PROFILE(SMBtranss2);
		return;
	}

	for (state = conn->pending_trans; state != NULL;
	     state = state->next) {
		if (state->mid == req->mid) {
			break;
		}
	}

	if ((state == NULL) || (state->cmd != SMBtrans2)) {
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		END_PROFILE(SMBtranss2);
		return;
	}

	/* Revise state->total_param and state->total_data in case they have
	   changed downwards */

	if (SVAL(req->vwv+0, 0) < state->total_param)
		state->total_param = SVAL(req->vwv+0, 0);
	if (SVAL(req->vwv+1, 0) < state->total_data)
		state->total_data = SVAL(req->vwv+1, 0);

	pcnt = SVAL(req->vwv+2, 0);
	poff = SVAL(req->vwv+3, 0);
	pdisp = SVAL(req->vwv+4, 0);

	dcnt = SVAL(req->vwv+5, 0);
	doff = SVAL(req->vwv+6, 0);
	ddisp = SVAL(req->vwv+7, 0);

	state->received_param += pcnt;
	state->received_data += dcnt;

	if ((state->received_data > state->total_data) ||
	    (state->received_param > state->total_param))
		goto bad_param;

	if (pcnt) {
		if (trans_oob(state->total_param, pdisp, pcnt)
		    || trans_oob(smb_len(req->inbuf), poff, pcnt)) {
			goto bad_param;
		}
		memcpy(state->param+pdisp,smb_base(req->inbuf)+poff,pcnt);
	}

	if (dcnt) {
		if (trans_oob(state->total_data, ddisp, dcnt)
		    || trans_oob(smb_len(req->inbuf), doff, dcnt)) {
			goto bad_param;
		}
		memcpy(state->data+ddisp, smb_base(req->inbuf)+doff,dcnt);
	}

	if ((state->received_param < state->total_param) ||
	    (state->received_data < state->total_data)) {
		END_PROFILE(SMBtranss2);
		return;
	}

	handle_trans2(conn, req, state);

	DLIST_REMOVE(conn->pending_trans, state);
	SAFE_FREE(state->data);
	SAFE_FREE(state->param);
	TALLOC_FREE(state);

	END_PROFILE(SMBtranss2);
	return;

  bad_param:

	DEBUG(0,("reply_transs2: invalid trans parameters\n"));
	DLIST_REMOVE(conn->pending_trans, state);
	SAFE_FREE(state->data);
	SAFE_FREE(state->param);
	TALLOC_FREE(state);
	reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
	END_PROFILE(SMBtranss2);
	return;
}