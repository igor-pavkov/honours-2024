static void avrcp_volume_changed(struct avrcp *session,
						struct avrcp_header *pdu)
{
	struct avrcp_player *player = target_get_player(session);
	int8_t volume;

	volume = pdu->params[1] & 0x7F;

	/* Always attempt to update the transport volume */
	media_transport_update_device_volume(session->dev, volume);

	if (player)
		player->cb->set_volume(volume, session->dev, player->user_data);
}