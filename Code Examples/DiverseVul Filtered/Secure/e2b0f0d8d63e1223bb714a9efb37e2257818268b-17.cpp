static int ct_pause(struct media_player *mp, void *user_data)
{
	struct avrcp_player *player = user_data;

	return ct_press(player, AVC_PAUSE);
}