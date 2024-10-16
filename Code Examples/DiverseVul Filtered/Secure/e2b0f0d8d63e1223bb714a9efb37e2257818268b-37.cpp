static void avrcp_search(struct avrcp *session, const char *string)
{
	uint8_t buf[AVRCP_BROWSING_HEADER_LENGTH + 255];
	struct avrcp_browsing_header *pdu = (void *) buf;
	uint16_t len, stringlen;

	memset(buf, 0, sizeof(buf));
	len = AVRCP_BROWSING_HEADER_LENGTH + 4;
	stringlen = strnlen(string, sizeof(buf) - len);
	len += stringlen;

	put_be16(AVRCP_CHARSET_UTF8, &pdu->params[0]);
	put_be16(stringlen, &pdu->params[2]);
	memcpy(&pdu->params[4], string, stringlen);
	pdu->pdu_id = AVRCP_SEARCH;
	pdu->param_len = htons(len - AVRCP_BROWSING_HEADER_LENGTH);

	avctp_send_browsing_req(session->conn, buf, len, avrcp_search_rsp,
								session);
}