static void avrcp_get_current_player_value(struct avrcp *session,
						uint8_t *attrs, uint8_t count)
{
	uint8_t buf[AVRCP_HEADER_LENGTH + AVRCP_ATTRIBUTE_LAST + 1];
	struct avrcp_header *pdu = (void *) buf;
	uint16_t length = AVRCP_HEADER_LENGTH + count + 1;

	memset(buf, 0, sizeof(buf));

	set_company_id(pdu->company_id, IEEEID_BTSIG);
	pdu->pdu_id = AVRCP_GET_CURRENT_PLAYER_VALUE;
	pdu->packet_type = AVRCP_PACKET_TYPE_SINGLE;
	pdu->params_len = htons(count + 1);
	pdu->params[0] = count;

	memcpy(pdu->params + 1, attrs, count);

	avctp_send_vendordep_req(session->conn, AVC_CTYPE_STATUS,
					AVC_SUBUNIT_PANEL, buf, length,
					avrcp_player_value_rsp, session);
}