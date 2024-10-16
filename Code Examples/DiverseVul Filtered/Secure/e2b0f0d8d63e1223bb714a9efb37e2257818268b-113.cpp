static void avrcp_get_capabilities(struct avrcp *session)
{
	uint8_t buf[AVRCP_HEADER_LENGTH + AVRCP_GET_CAPABILITIES_PARAM_LENGTH];
	struct avrcp_header *pdu = (void *) buf;
	uint8_t length;

	memset(buf, 0, sizeof(buf));

	set_company_id(pdu->company_id, IEEEID_BTSIG);
	pdu->pdu_id = AVRCP_GET_CAPABILITIES;
	pdu->packet_type = AVRCP_PACKET_TYPE_SINGLE;
	pdu->params[0] = CAP_EVENTS_SUPPORTED;
	pdu->params_len = htons(AVRCP_GET_CAPABILITIES_PARAM_LENGTH);

	length = AVRCP_HEADER_LENGTH + ntohs(pdu->params_len);

	avctp_send_vendordep_req(session->conn, AVC_CTYPE_STATUS,
					AVC_SUBUNIT_PANEL, buf, length,
					avrcp_get_capabilities_resp,
					session);
}