static void avrcp_available_players_changed(struct avrcp *session,
						struct avrcp_header *pdu)
{
	avrcp_get_media_player_list(session);
}