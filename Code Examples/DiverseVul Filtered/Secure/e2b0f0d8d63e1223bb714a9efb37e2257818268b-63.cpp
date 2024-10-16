static void avrcp_add_to_nowplaying(struct avrcp *session, uint64_t uid)
{
	uint8_t buf[AVRCP_HEADER_LENGTH + 11];
	struct avrcp_player *player = session->controller->player;
	struct avrcp_header *pdu = (void *) buf;
	uint16_t length;

	memset(buf, 0, sizeof(buf));

	set_company_id(pdu->company_id, IEEEID_BTSIG);
	pdu->pdu_id = AVRCP_ADD_TO_NOW_PLAYING;
	pdu->params_len = htons(11);
	pdu->packet_type = AVRCP_PACKET_TYPE_SINGLE;

	pdu->params[0] = player->scope;
	put_be64(uid, &pdu->params[1]);
	put_be16(player->uid_counter, &pdu->params[9]);

	length = AVRCP_HEADER_LENGTH + ntohs(pdu->params_len);

	avctp_send_vendordep_req(session->conn, AVC_CTYPE_CONTROL,
					AVC_SUBUNIT_PANEL, buf, length,
					NULL, session);
}