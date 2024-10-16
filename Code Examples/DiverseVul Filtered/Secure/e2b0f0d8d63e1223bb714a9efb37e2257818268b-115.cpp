static uint8_t avrcp_handle_request_continuing(struct avrcp *session,
						struct avrcp_header *pdu,
						uint8_t transaction)
{
	struct avrcp_player *player = target_get_player(session);
	uint16_t len = ntohs(pdu->params_len);
	struct pending_pdu *pending;

	if (len != 1 || session->pending_pdu == NULL)
		goto err;

	pending = session->pending_pdu;

	if (pending->pdu_id != pdu->params[0])
		goto err;

	len = 0;
	pending->attr_ids = player_fill_media_attribute(player,
							pending->attr_ids,
							pdu->params, &len,
							&pending->offset);
	pdu->pdu_id = pending->pdu_id;

	if (pending->attr_ids == NULL) {
		g_free(session->pending_pdu);
		session->pending_pdu = NULL;
		pdu->packet_type = AVRCP_PACKET_TYPE_END;
	} else {
		pdu->packet_type = AVRCP_PACKET_TYPE_CONTINUING;
	}

	pdu->params_len = htons(len);

	return AVC_CTYPE_STABLE;
err:
	pdu->params_len = htons(1);
	pdu->params[0] = AVRCP_STATUS_INVALID_PARAM;
	return AVC_CTYPE_REJECTED;
}