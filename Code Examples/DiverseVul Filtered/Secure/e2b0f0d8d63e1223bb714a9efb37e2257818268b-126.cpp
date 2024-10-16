static gboolean notify_addressed_player_changed(gpointer user_data)
{
	struct avrcp_player *player = user_data;
	uint8_t events[6] = { AVRCP_EVENT_STATUS_CHANGED,
					AVRCP_EVENT_TRACK_CHANGED,
					AVRCP_EVENT_TRACK_REACHED_START,
					AVRCP_EVENT_TRACK_REACHED_END,
					AVRCP_EVENT_SETTINGS_CHANGED,
					AVRCP_EVENT_PLAYBACK_POS_CHANGED
				};
	uint8_t i;

	avrcp_player_event(player, AVRCP_EVENT_ADDRESSED_PLAYER_CHANGED, NULL);

	/*
	 * TG shall complete all player specific
	 * notifications with AV/C C-Type REJECTED
	 * with error code as Addressed Player Changed.
	 */
	for (i = 0; i < sizeof(events); i++)
		avrcp_player_event(player, events[i], NULL);

	player->changed_id = 0;

	return FALSE;
}