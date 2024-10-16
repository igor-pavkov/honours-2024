static int ct_release_key(struct media_player *mp, void *user_data)
{
	struct avrcp_player *player = user_data;

	return ct_release(player);
}