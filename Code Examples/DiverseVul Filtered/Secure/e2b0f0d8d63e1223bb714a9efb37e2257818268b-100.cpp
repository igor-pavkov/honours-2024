static void avrcp_list_items(struct avrcp *session, uint32_t start,
								uint32_t end)
{
	uint8_t buf[AVRCP_BROWSING_HEADER_LENGTH + 10 +
			AVRCP_MEDIA_ATTRIBUTE_LAST * sizeof(uint32_t)];
	struct avrcp_player *player = session->controller->player;
	struct avrcp_browsing_header *pdu = (void *) buf;
	uint16_t length = AVRCP_BROWSING_HEADER_LENGTH + 10;
	uint32_t attribute;

	memset(buf, 0, sizeof(buf));

	pdu->pdu_id = AVRCP_GET_FOLDER_ITEMS;
	pdu->param_len = htons(10 + sizeof(uint32_t));

	pdu->params[0] = player->scope;

	put_be32(start, &pdu->params[1]);
	put_be32(end, &pdu->params[5]);

	pdu->params[9] = 1;

	/* Only the title (0x01) is mandatory. This can be extended to
	 * support AVRCP_MEDIA_ATTRIBUTE_* attributes */
	attribute = htonl(AVRCP_MEDIA_ATTRIBUTE_TITLE);
	memcpy(&pdu->params[10], &attribute, sizeof(uint32_t));

	length += sizeof(uint32_t);

	avctp_send_browsing_req(session->conn, buf, length,
					avrcp_list_items_rsp, session);
}