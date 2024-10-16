static void avrcp_set_addressed_player(struct avrcp *session,
						struct avrcp_player *player)
{
	uint8_t buf[AVRCP_HEADER_LENGTH + 2];
	struct avrcp_header *pdu = (void *) buf;
	uint16_t id;

	memset(buf, 0, sizeof(buf));

	set_company_id(pdu->company_id, IEEEID_BTSIG);
	pdu->pdu_id = AVRCP_SET_ADDRESSED_PLAYER;
	pdu->packet_type = AVRCP_PACKET_TYPE_SINGLE;
	id = htons(player->id);
	memcpy(pdu->params, &id, 2);
	pdu->params_len = htons(2);

	avctp_send_vendordep_req(session->conn, AVC_CTYPE_CONTROL,
					AVC_SUBUNIT_PANEL, buf, sizeof(buf),
					avrcp_set_addressed_player_rsp,
					session);
}