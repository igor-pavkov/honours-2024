static void avrcp_register_notification(struct avrcp *session, uint8_t event)
{
	uint8_t buf[AVRCP_HEADER_LENGTH + AVRCP_REGISTER_NOTIFICATION_PARAM_LENGTH];
	struct avrcp_header *pdu = (void *) buf;
	uint8_t length;

	memset(buf, 0, sizeof(buf));

	set_company_id(pdu->company_id, IEEEID_BTSIG);
	pdu->pdu_id = AVRCP_REGISTER_NOTIFICATION;
	pdu->packet_type = AVRCP_PACKET_TYPE_SINGLE;
	pdu->params[0] = event;

	/*
	 * Set maximum interval possible for position changed as we only
	 * use it to resync.
	 */
	if (event == AVRCP_EVENT_PLAYBACK_POS_CHANGED)
		bt_put_be32(UINT32_MAX / 1000, &pdu->params[1]);

	pdu->params_len = htons(AVRCP_REGISTER_NOTIFICATION_PARAM_LENGTH);

	length = AVRCP_HEADER_LENGTH + ntohs(pdu->params_len);

	avctp_send_vendordep_req(session->conn, AVC_CTYPE_NOTIFY,
					AVC_SUBUNIT_PANEL, buf, length,
					avrcp_handle_event, session);
}