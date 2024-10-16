static int player_set_setting(struct avrcp_player *player, uint8_t id,
								uint8_t val)
{
	const char *key, *value;

	key = attr_to_str(id);
	if (key == NULL)
		return -EINVAL;

	value = attrval_to_str(id, val);
	if (value == NULL)
		return -EINVAL;

	if (player == NULL)
		return -ENOENT;

	return player->cb->set_setting(key, value, player->user_data);
}