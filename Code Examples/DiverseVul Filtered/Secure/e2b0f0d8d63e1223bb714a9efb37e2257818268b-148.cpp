static void avrcp_player_parse_features(struct avrcp_player *player,
							uint8_t *features)
{
	struct media_player *mp = player->user_data;

	player->features = g_memdup(features, 16);

	if (features[7] & 0x08) {
		media_player_set_browsable(mp, true);
		media_player_create_folder(mp, "/Filesystem",
						PLAYER_FOLDER_TYPE_MIXED, 0);
	}

	if (features[7] & 0x10)
		media_player_set_searchable(mp, true);

	if (features[8] & 0x02) {
		media_player_create_folder(mp, "/NowPlaying",
						PLAYER_FOLDER_TYPE_MIXED, 0);
		media_player_set_playlist(mp, "/NowPlaying");
	}
}