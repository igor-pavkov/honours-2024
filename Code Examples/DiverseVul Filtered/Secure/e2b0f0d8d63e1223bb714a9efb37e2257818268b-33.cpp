static void avrcp_get_play_status(struct avrcp *session)
{
	uint8_t buf[AVRCP_HEADER_LENGTH];
	struct avrcp_header *pdu = (void *) buf;

	memset(buf, 0, sizeof(buf));

	set_company_id(pdu->company_id, IEEEID_BTSIG);
	pdu->pdu_id = AVRCP_GET_PLAY_STATUS;
	pdu->packet_type = AVRCP_PACKET_TYPE_SINGLE;

	avctp_send_vendordep_req(session->conn, AVC_CTYPE_STATUS,
					AVC_SUBUNIT_PANEL, buf, sizeof(buf),
					avrcp_get_play_status_rsp,
					session);
}