static gboolean avrcp_list_player_attributes_rsp(struct avctp *conn,
					uint8_t code, uint8_t subunit,
					uint8_t transaction, uint8_t *operands,
					size_t operand_count, void *user_data)
{
	uint8_t attrs[AVRCP_ATTRIBUTE_LAST];
	struct avrcp *session = user_data;
	struct avrcp_header *pdu = (void *) operands;
	uint8_t len, count = 0;
	int i;

	if (code == AVC_CTYPE_REJECTED || code == AVC_CTYPE_NOT_IMPLEMENTED)
		return FALSE;

	len = pdu->params[0];

	if (ntohs(pdu->params_len) < count) {
		error("Invalid parameters");
		return FALSE;
	}

	for (i = 0; len > 0; len--, i++) {
		/* Don't query invalid attributes */
		if (pdu->params[i + 1] == AVRCP_ATTRIBUTE_ILEGAL ||
				pdu->params[i + 1] > AVRCP_ATTRIBUTE_LAST)
			continue;

		attrs[count++] = pdu->params[i + 1];
	}

	avrcp_get_current_player_value(session, attrs, count);

	return FALSE;
}