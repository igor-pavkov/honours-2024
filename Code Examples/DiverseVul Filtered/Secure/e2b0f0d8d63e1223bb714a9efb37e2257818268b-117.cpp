static GList *player_list_metadata(struct avrcp_player *player)
{
	GList *l, *attrs = NULL;

	if (player == NULL)
		return g_list_prepend(NULL,
				GUINT_TO_POINTER(AVRCP_MEDIA_ATTRIBUTE_TITLE));

	l = player->cb->list_metadata(player->user_data);
	for (; l; l = l->next) {
		const char *key = l->data;

		attrs = g_list_append(attrs,
					GUINT_TO_POINTER(str_to_metadata(key)));
	}

	return attrs;
}