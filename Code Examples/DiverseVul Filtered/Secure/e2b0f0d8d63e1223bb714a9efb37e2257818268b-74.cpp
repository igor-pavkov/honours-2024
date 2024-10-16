size_t avrcp_browsing_general_reject(uint8_t *operands)
{
	struct avrcp_browsing_header *pdu = (void *) operands;
	uint8_t status;

	pdu->pdu_id = AVRCP_GENERAL_REJECT;
	status = AVRCP_STATUS_INVALID_COMMAND;

	pdu->param_len = htons(sizeof(status));
	memcpy(pdu->params, &status, (sizeof(status)));
	return AVRCP_BROWSING_HEADER_LENGTH + sizeof(status);
}