static const char *player_get_metadata(struct avrcp_player *player,
								uint32_t id)
{
	const char *key;

	key = metadata_to_str(id);
	if (key == NULL)
		return NULL;

	if (player != NULL)
		return player->cb->get_metadata(key, player->user_data);

	if (id == AVRCP_MEDIA_ATTRIBUTE_TITLE)
		return "";

	return NULL;
}