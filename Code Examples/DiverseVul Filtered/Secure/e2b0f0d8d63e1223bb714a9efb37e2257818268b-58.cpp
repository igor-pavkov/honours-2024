static gboolean avrcp_handle_set_volume(struct avctp *conn, uint8_t code,
					uint8_t subunit, uint8_t transaction,
					uint8_t *operands, size_t operand_count,
					void *user_data)
{
	struct avrcp *session = user_data;
	struct avrcp_player *player = target_get_player(session);
	struct avrcp_header *pdu = (void *) operands;
	int8_t volume;

	if (code == AVC_CTYPE_REJECTED || code == AVC_CTYPE_NOT_IMPLEMENTED ||
								pdu == NULL)
		return FALSE;

	volume = pdu->params[0] & 0x7F;

	/* Always attempt to update the transport volume */
	media_transport_update_device_volume(session->dev, volume);

	if (player != NULL)
		player->cb->set_volume(volume, session->dev, player->user_data);

	return FALSE;
}