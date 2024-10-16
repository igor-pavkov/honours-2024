static int ct_press_key(struct media_player *mp, uint8_t avc_key,
								void *user_data)
{
	struct avrcp_player *player = user_data;

	return ct_press(player, avc_key);
}