static uint8_t avrcp_handle_get_capabilities(struct avrcp *session,
						struct avrcp_header *pdu,
						uint8_t transaction)
{
	uint16_t len = ntohs(pdu->params_len);
	unsigned int i;

	if (len != 1)
		goto err;

	DBG("id=%u", pdu->params[0]);

	switch (pdu->params[0]) {
	case CAP_COMPANY_ID:
		for (i = 0; i < G_N_ELEMENTS(company_ids); i++) {
			set_company_id(&pdu->params[2 + i * 3],
							company_ids[i]);
		}

		pdu->params_len = htons(2 + (3 * G_N_ELEMENTS(company_ids)));
		pdu->params[1] = G_N_ELEMENTS(company_ids);

		return AVC_CTYPE_STABLE;
	case CAP_EVENTS_SUPPORTED:
		pdu->params[1] = 0;
		for (i = 1; i <= AVRCP_EVENT_LAST; i++) {
			if (session->supported_events & (1 << i)) {
				pdu->params[1]++;
				pdu->params[pdu->params[1] + 1] = i;
			}
		}

		pdu->params_len = htons(2 + pdu->params[1]);
		return AVC_CTYPE_STABLE;
	}

err:
	pdu->params_len = htons(1);
	pdu->params[0] = AVRCP_STATUS_INVALID_PARAM;

	return AVC_CTYPE_REJECTED;
}