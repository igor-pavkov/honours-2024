static int player_get_setting(struct avrcp_player *player, uint8_t id)
{
	const char *key;
	const char *value;

	if (player == NULL)
		return -ENOENT;

	key = attr_to_str(id);
	if (key == NULL)
		return -EINVAL;

	value = player->cb->get_setting(key, player->user_data);
	if (value == NULL)
		return -EINVAL;

	return attrval_to_val(id, value);
}