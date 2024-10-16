static void avrcp_get_media_player_list(struct avrcp *session)
{
	uint8_t buf[AVRCP_BROWSING_HEADER_LENGTH + 10];
	struct avrcp_browsing_header *pdu = (void *) buf;

	memset(buf, 0, sizeof(buf));

	pdu->pdu_id = AVRCP_GET_FOLDER_ITEMS;
	put_be32(0, &pdu->params[1]);
	put_be32(UINT32_MAX, &pdu->params[5]);
	pdu->param_len = htons(10);

	avctp_send_browsing_req(session->conn, buf, sizeof(buf),
				avrcp_get_media_player_list_rsp, session);
}