static void avrcp_set_player_value(struct avrcp *session, uint8_t attr,
								uint8_t val)
{
	uint8_t buf[AVRCP_HEADER_LENGTH + 3];
	struct avrcp_header *pdu = (void *) buf;
	uint8_t length;

	memset(buf, 0, sizeof(buf));

	set_company_id(pdu->company_id, IEEEID_BTSIG);
	pdu->pdu_id = AVRCP_SET_PLAYER_VALUE;
	pdu->packet_type = AVRCP_PACKET_TYPE_SINGLE;
	pdu->params[0] = 1;
	pdu->params[1] = attr;
	pdu->params[2] = val;
	pdu->params_len = htons(3);

	length = AVRCP_HEADER_LENGTH + ntohs(pdu->params_len);

	avctp_send_vendordep_req(session->conn, AVC_CTYPE_CONTROL,
					AVC_SUBUNIT_PANEL, buf, length,
					avrcp_player_value_rsp, session);
}