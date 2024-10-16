static int ct_fast_forward(struct media_player *mp, void *user_data)
{
	struct avrcp_player *player = user_data;

	return ct_hold(player, AVC_FAST_FORWARD);
}