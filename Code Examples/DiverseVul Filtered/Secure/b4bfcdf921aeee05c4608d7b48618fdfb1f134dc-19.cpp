static void send_nt_replies(connection_struct *conn,
			    struct smb_request *req, NTSTATUS nt_error,
			    char *params, int paramsize,
			    char *pdata, int datasize)
{
	int data_to_send = datasize;
	int params_to_send = paramsize;
	int useable_space;
	char *pp = params;
	char *pd = pdata;
	int params_sent_thistime, data_sent_thistime, total_sent_thistime;
	int alignment_offset = 1;
	int data_alignment_offset = 0;
	struct smbd_server_connection *sconn = req->sconn;
	int max_send = sconn->smb1.sessions.max_send;

	/*
	 * If there genuinely are no parameters or data to send just send
	 * the empty packet.
	 */

	if(params_to_send == 0 && data_to_send == 0) {
		reply_outbuf(req, 18, 0);
		if (NT_STATUS_V(nt_error)) {
			error_packet_set((char *)req->outbuf,
					 0, 0, nt_error,
					 __LINE__,__FILE__);
		}
		show_msg((char *)req->outbuf);
		if (!srv_send_smb(sconn,
				(char *)req->outbuf,
				true, req->seqnum+1,
				IS_CONN_ENCRYPTED(conn),
				&req->pcd)) {
			exit_server_cleanly("send_nt_replies: srv_send_smb failed.");
		}
		TALLOC_FREE(req->outbuf);
		return;
	}

	/*
	 * When sending params and data ensure that both are nicely aligned.
	 * Only do this alignment when there is also data to send - else
	 * can cause NT redirector problems.
	 */

	if (((params_to_send % 4) != 0) && (data_to_send != 0)) {
		data_alignment_offset = 4 - (params_to_send % 4);
	}

	/*
	 * Space is bufsize minus Netbios over TCP header minus SMB header.
	 * The alignment_offset is to align the param bytes on a four byte
	 * boundary (2 bytes for data len, one byte pad).
	 * NT needs this to work correctly.
	 */

	useable_space = max_send - (smb_size
				    + 2 * 18 /* wct */
				    + alignment_offset
				    + data_alignment_offset);

	if (useable_space < 0) {
		char *msg = talloc_asprintf(
			talloc_tos(),
			"send_nt_replies failed sanity useable_space = %d!!!",
			useable_space);
		DEBUG(0, ("%s\n", msg));
		exit_server_cleanly(msg);
	}

	while (params_to_send || data_to_send) {

		/*
		 * Calculate whether we will totally or partially fill this packet.
		 */

		total_sent_thistime = params_to_send + data_to_send;

		/*
		 * We can never send more than useable_space.
		 */

		total_sent_thistime = MIN(total_sent_thistime, useable_space);

		reply_outbuf(req, 18,
			     total_sent_thistime + alignment_offset
			     + data_alignment_offset);

		/*
		 * Set total params and data to be sent.
		 */

		SIVAL(req->outbuf,smb_ntr_TotalParameterCount,paramsize);
		SIVAL(req->outbuf,smb_ntr_TotalDataCount,datasize);

		/*
		 * Calculate how many parameters and data we can fit into
		 * this packet. Parameters get precedence.
		 */

		params_sent_thistime = MIN(params_to_send,useable_space);
		data_sent_thistime = useable_space - params_sent_thistime;
		data_sent_thistime = MIN(data_sent_thistime,data_to_send);

		SIVAL(req->outbuf, smb_ntr_ParameterCount,
		      params_sent_thistime);

		if(params_sent_thistime == 0) {
			SIVAL(req->outbuf,smb_ntr_ParameterOffset,0);
			SIVAL(req->outbuf,smb_ntr_ParameterDisplacement,0);
		} else {
			/*
			 * smb_ntr_ParameterOffset is the offset from the start of the SMB header to the
			 * parameter bytes, however the first 4 bytes of outbuf are
			 * the Netbios over TCP header. Thus use smb_base() to subtract
			 * them from the calculation.
			 */

			SIVAL(req->outbuf,smb_ntr_ParameterOffset,
			      ((smb_buf(req->outbuf)+alignment_offset)
			       - smb_base(req->outbuf)));
			/*
			 * Absolute displacement of param bytes sent in this packet.
			 */

			SIVAL(req->outbuf, smb_ntr_ParameterDisplacement,
			      pp - params);
		}

		/*
		 * Deal with the data portion.
		 */

		SIVAL(req->outbuf, smb_ntr_DataCount, data_sent_thistime);

		if(data_sent_thistime == 0) {
			SIVAL(req->outbuf,smb_ntr_DataOffset,0);
			SIVAL(req->outbuf,smb_ntr_DataDisplacement, 0);
		} else {
			/*
			 * The offset of the data bytes is the offset of the
			 * parameter bytes plus the number of parameters being sent this time.
			 */

			SIVAL(req->outbuf, smb_ntr_DataOffset,
			      ((smb_buf(req->outbuf)+alignment_offset) -
			       smb_base(req->outbuf))
			      + params_sent_thistime + data_alignment_offset);
			SIVAL(req->outbuf,smb_ntr_DataDisplacement, pd - pdata);
		}

		/*
		 * Copy the param bytes into the packet.
		 */

		if(params_sent_thistime) {
			if (alignment_offset != 0) {
				memset(smb_buf(req->outbuf), 0,
				       alignment_offset);
			}
			memcpy((smb_buf(req->outbuf)+alignment_offset), pp,
			       params_sent_thistime);
		}

		/*
		 * Copy in the data bytes
		 */

		if(data_sent_thistime) {
			if (data_alignment_offset != 0) {
				memset((smb_buf(req->outbuf)+alignment_offset+
					params_sent_thistime), 0,
				       data_alignment_offset);
			}
			memcpy(smb_buf(req->outbuf)+alignment_offset
			       +params_sent_thistime+data_alignment_offset,
			       pd,data_sent_thistime);
		}

		DEBUG(9,("nt_rep: params_sent_thistime = %d, data_sent_thistime = %d, useable_space = %d\n",
			params_sent_thistime, data_sent_thistime, useable_space));
		DEBUG(9,("nt_rep: params_to_send = %d, data_to_send = %d, paramsize = %d, datasize = %d\n",
			params_to_send, data_to_send, paramsize, datasize));

		if (NT_STATUS_V(nt_error)) {
			error_packet_set((char *)req->outbuf,
					 0, 0, nt_error,
					 __LINE__,__FILE__);
		}

		/* Send the packet */
		show_msg((char *)req->outbuf);
		if (!srv_send_smb(sconn,
				(char *)req->outbuf,
				true, req->seqnum+1,
				IS_CONN_ENCRYPTED(conn),
				&req->pcd)) {
			exit_server_cleanly("send_nt_replies: srv_send_smb failed.");
		}

		TALLOC_FREE(req->outbuf);

		pp += params_sent_thistime;
		pd += data_sent_thistime;

		params_to_send -= params_sent_thistime;
		data_to_send -= data_sent_thistime;

		/*
		 * Sanity check
		 */

		if(params_to_send < 0 || data_to_send < 0) {
			DEBUG(0,("send_nt_replies failed sanity check pts = %d, dts = %d\n!!!",
				params_to_send, data_to_send));
			exit_server_cleanly("send_nt_replies: internal error");
		}
	}
}