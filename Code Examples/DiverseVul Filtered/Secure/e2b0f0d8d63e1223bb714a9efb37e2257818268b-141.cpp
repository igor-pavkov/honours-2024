static bool ct_set_setting(struct media_player *mp, const char *key,
					const char *value, void *user_data)
{
	struct avrcp_player *player = user_data;
	int attr;
	int val;
	struct avrcp *session;

	session = player->sessions->data;
	if (session == NULL)
		return false;

	if (session->controller->version < 0x0103)
		return false;

	set_ct_player(session, player);

	attr = attr_to_val(key);
	if (attr < 0)
		return false;

	val = attrval_to_val(attr, value);
	if (val < 0)
		return false;

	avrcp_set_player_value(session, attr, val);

	return true;
}