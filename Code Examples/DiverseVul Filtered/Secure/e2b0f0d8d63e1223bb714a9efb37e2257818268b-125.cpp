static uint8_t avrcp_handle_abort_continuing(struct avrcp *session,
						struct avrcp_header *pdu,
						uint8_t transaction)
{
	uint16_t len = ntohs(pdu->params_len);
	struct pending_pdu *pending;

	if (len != 1 || session->pending_pdu == NULL)
		goto err;

	pending = session->pending_pdu;

	if (pending->pdu_id != pdu->params[0])
		goto err;

	session_abort_pending_pdu(session);
	pdu->params_len = 0;

	return AVC_CTYPE_ACCEPTED;

err:
	pdu->params_len = htons(1);
	pdu->params[0] = AVRCP_STATUS_INVALID_PARAM;
	return AVC_CTYPE_REJECTED;
}