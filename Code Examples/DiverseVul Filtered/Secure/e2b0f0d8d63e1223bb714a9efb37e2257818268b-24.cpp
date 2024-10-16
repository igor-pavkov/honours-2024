static uint8_t avrcp_handle_set_absolute_volume(struct avrcp *session,
						struct avrcp_header *pdu,
						uint8_t transaction)
{
	uint16_t len = ntohs(pdu->params_len);
	int8_t volume;

	if (len != 1)
		goto err;

	volume = pdu->params[0] & 0x7F;

	media_transport_update_device_volume(session->dev, volume);

	return AVC_CTYPE_ACCEPTED;

err:
	pdu->params_len = htons(1);
	pdu->params[0] = AVRCP_STATUS_INVALID_PARAM;
	return AVC_CTYPE_REJECTED;
}