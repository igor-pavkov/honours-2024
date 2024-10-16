void reply_nttranss(struct smb_request *req)
{
	connection_struct *conn = req->conn;
	uint32_t pcnt,poff,dcnt,doff,pdisp,ddisp;
	struct trans_state *state;

	START_PROFILE(SMBnttranss);

	show_msg((const char *)req->inbuf);

	/* Windows clients expect all replies to
	   an NT transact secondary (SMBnttranss 0xA1)
	   to have a command code of NT transact
	   (SMBnttrans 0xA0). See bug #8989 for details. */
	req->cmd = SMBnttrans;

	if (req->wct < 18) {
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		END_PROFILE(SMBnttranss);
		return;
	}

	for (state = conn->pending_trans; state != NULL;
	     state = state->next) {
		if (state->mid == req->mid) {
			break;
		}
	}

	if ((state == NULL) || (state->cmd != SMBnttrans)) {
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		END_PROFILE(SMBnttranss);
		return;
	}

	/* Revise state->total_param and state->total_data in case they have
	   changed downwards */
	if (IVAL(req->vwv+1, 1) < state->total_param) {
		state->total_param = IVAL(req->vwv+1, 1);
	}
	if (IVAL(req->vwv+3, 1) < state->total_data) {
		state->total_data = IVAL(req->vwv+3, 1);
	}

	pcnt = IVAL(req->vwv+5, 1);
	poff = IVAL(req->vwv+7, 1);
	pdisp = IVAL(req->vwv+9, 1);

	dcnt = IVAL(req->vwv+11, 1);
	doff = IVAL(req->vwv+13, 1);
	ddisp = IVAL(req->vwv+15, 1);

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
		memcpy(state->param+pdisp, smb_base(req->inbuf)+poff,pcnt);
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
		END_PROFILE(SMBnttranss);
		return;
	}

	handle_nttrans(conn, state, req);

	DLIST_REMOVE(conn->pending_trans, state);
	SAFE_FREE(state->data);
	SAFE_FREE(state->param);
	TALLOC_FREE(state);
	END_PROFILE(SMBnttranss);
	return;

  bad_param:

	DEBUG(0,("reply_nttranss: invalid trans parameters\n"));
	DLIST_REMOVE(conn->pending_trans, state);
	SAFE_FREE(state->data);
	SAFE_FREE(state->param);
	TALLOC_FREE(state);
	reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
	END_PROFILE(SMBnttranss);
	return;
}