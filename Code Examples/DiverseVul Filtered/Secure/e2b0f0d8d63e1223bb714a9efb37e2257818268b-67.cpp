static size_t handle_browsing_pdu(struct avctp *conn,
					uint8_t transaction, uint8_t *operands,
					size_t operand_count, void *user_data)
{
	struct avrcp *session = user_data;
	struct browsing_pdu_handler *handler;
	struct avrcp_browsing_header *pdu = (void *) operands;

	DBG("AVRCP Browsing PDU 0x%02X, len 0x%04X", pdu->pdu_id,
							ntohs(pdu->param_len));

	for (handler = browsing_handlers; handler->pdu_id; handler++) {
		if (handler->pdu_id == pdu->pdu_id)
			goto done;
	}

	return avrcp_browsing_general_reject(operands);

done:
	session->transaction = transaction;
	handler->func(session, pdu, transaction);
	return AVRCP_BROWSING_HEADER_LENGTH + ntohs(pdu->param_len);
}