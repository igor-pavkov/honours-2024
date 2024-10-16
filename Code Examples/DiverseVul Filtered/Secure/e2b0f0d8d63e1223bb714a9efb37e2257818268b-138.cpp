static void avrcp_set_browsed_player(struct avrcp *session,
						struct avrcp_player *player)
{
	uint8_t buf[AVRCP_BROWSING_HEADER_LENGTH + 2];
	struct avrcp_browsing_header *pdu = (void *) buf;
	uint16_t id;

	memset(buf, 0, sizeof(buf));

	pdu->pdu_id = AVRCP_SET_BROWSED_PLAYER;
	id = htons(player->id);
	memcpy(pdu->params, &id, 2);
	pdu->param_len = htons(2);

	avctp_send_browsing_req(session->conn, buf, sizeof(buf),
				avrcp_set_browsed_player_rsp, session);
}