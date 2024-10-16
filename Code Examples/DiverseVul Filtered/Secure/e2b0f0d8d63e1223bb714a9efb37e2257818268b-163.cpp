static void avrcp_handle_media_player_list(struct avrcp *session,
				struct avrcp_browsing_header *pdu,
				uint32_t start_item, uint32_t end_item)
{
	struct avrcp_player *player = session->target->player;
	struct get_folder_items_rsp *rsp;
	const char *name = NULL;
	GSList *l;

	rsp = (void *)pdu->params;
	rsp->status = AVRCP_STATUS_SUCCESS;
	rsp->uid_counter = htons(player_get_uid_counter(player));
	rsp->num_items = 0;
	pdu->param_len = sizeof(*rsp);

	for (l = g_slist_nth(session->server->players, start_item);
					l; l = g_slist_next(l)) {
		struct avrcp_player *player = l->data;
		struct folder_item *folder;
		struct player_item *item;
		uint16_t namelen;

		if (rsp->num_items == (end_item - start_item) + 1)
			break;

		folder = (void *)&pdu->params[pdu->param_len];
		folder->type = 0x01; /* Media Player */

		pdu->param_len += sizeof(*folder);

		item = (void *)folder->data;
		item->player_id = htons(player->id);
		item->type = 0x01; /* Audio */
		item->subtype = htonl(0x01); /* Audio Book */
		item->status = player_get_status(player);
		/* Assign Default Feature Bit Mask */
		memcpy(&item->features, &default_features,
					sizeof(default_features));

		item->charset = htons(AVRCP_CHARSET_UTF8);

		name = player->cb->get_name(player->user_data);
		namelen = strlen(name);
		item->namelen = htons(namelen);
		memcpy(item->name, name, namelen);

		folder->len = htons(sizeof(*item) + namelen);
		pdu->param_len += sizeof(*item) + namelen;
		rsp->num_items++;
	}

	/* If no player could be found respond with an error */
	if (!rsp->num_items)
		goto failed;

	rsp->num_items = htons(rsp->num_items);
	pdu->param_len = htons(pdu->param_len);

	return;

failed:
	pdu->params[0] = AVRCP_STATUS_OUT_OF_BOUNDS;
	pdu->param_len = htons(1);
}