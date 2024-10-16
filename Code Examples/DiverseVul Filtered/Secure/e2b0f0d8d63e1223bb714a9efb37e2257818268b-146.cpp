static void avrcp_get_element_attributes(struct avrcp *session)
{
	uint8_t buf[AVRCP_HEADER_LENGTH + 9];
	struct avrcp_header *pdu = (void *) buf;
	uint16_t length;

	memset(buf, 0, sizeof(buf));

	set_company_id(pdu->company_id, IEEEID_BTSIG);
	pdu->pdu_id = AVRCP_GET_ELEMENT_ATTRIBUTES;
	pdu->params_len = htons(9);
	pdu->packet_type = AVRCP_PACKET_TYPE_SINGLE;

	length = AVRCP_HEADER_LENGTH + ntohs(pdu->params_len);

	avctp_send_vendordep_req(session->conn, AVC_CTYPE_STATUS,
					AVC_SUBUNIT_PANEL, buf, length,
					avrcp_get_element_attributes_rsp,
					session);
}