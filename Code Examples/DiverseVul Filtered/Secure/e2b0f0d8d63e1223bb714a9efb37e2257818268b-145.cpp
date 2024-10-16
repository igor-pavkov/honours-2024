static void avrcp_get_item_attributes(struct avrcp *session, uint64_t uid)
{
	struct avrcp_player *player = session->controller->player;
	uint8_t buf[AVRCP_BROWSING_HEADER_LENGTH + 12];
	struct avrcp_browsing_header *pdu = (void *) buf;

	memset(buf, 0, sizeof(buf));

	pdu->pdu_id = AVRCP_GET_ITEM_ATTRIBUTES;
	pdu->params[0] = 0x03;
	put_be64(uid, &pdu->params[1]);
	put_be16(player->uid_counter, &pdu->params[9]);
	pdu->param_len = htons(12);

	avctp_send_browsing_req(session->conn, buf, sizeof(buf),
				avrcp_get_item_attributes_rsp, session);
}