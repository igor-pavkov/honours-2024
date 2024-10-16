static void avrcp_get_total_numberofitems(struct avrcp *session)
{
	uint8_t buf[AVRCP_BROWSING_HEADER_LENGTH + 7];
	struct avrcp_player *player = session->controller->player;
	struct avrcp_browsing_header *pdu = (void *) buf;

	memset(buf, 0, sizeof(buf));

	pdu->pdu_id = AVRCP_GET_TOTAL_NUMBER_OF_ITEMS;
	pdu->param_len = htons(7 + sizeof(uint32_t));

	pdu->params[0] = player->scope;

	avctp_send_browsing_req(session->conn, buf, sizeof(buf),
				avrcp_get_total_numberofitems_rsp, session);
}