static void avrcp_change_path(struct avrcp *session, uint8_t direction,
								uint64_t uid)
{
	struct avrcp_player *player = session->controller->player;
	uint8_t buf[AVRCP_BROWSING_HEADER_LENGTH + 11];
	struct avrcp_browsing_header *pdu = (void *) buf;

	memset(buf, 0, sizeof(buf));
	put_be16(player->uid_counter, &pdu->params[0]);
	pdu->params[2] = direction;
	put_be64(uid, &pdu->params[3]);
	pdu->pdu_id = AVRCP_CHANGE_PATH;
	pdu->param_len = htons(11);

	avctp_send_browsing_req(session->conn, buf, sizeof(buf),
					avrcp_change_path_rsp, session);
}