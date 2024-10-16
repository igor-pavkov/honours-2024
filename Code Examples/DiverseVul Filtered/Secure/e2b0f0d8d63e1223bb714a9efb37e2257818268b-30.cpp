static int ct_play(struct media_player *mp, void *user_data)
{
	struct avrcp_player *player = user_data;

	return ct_press(player, AVC_PLAY);
}