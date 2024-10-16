size_t avrcp_handle_vendor_reject(uint8_t *code, uint8_t *operands)
{
	struct avrcp_header *pdu = (void *) operands;
	uint32_t company_id = get_company_id(pdu->company_id);

	*code = AVC_CTYPE_REJECTED;
	pdu->params_len = htons(1);
	pdu->params[0] = AVRCP_STATUS_INTERNAL_ERROR;

	DBG("rejecting AVRCP PDU 0x%02X, company 0x%06X len 0x%04X",
			pdu->pdu_id, company_id, ntohs(pdu->params_len));

	return AVRCP_HEADER_LENGTH + 1;
}