static void avrcp_handle_media_player_list_num_items(struct avrcp *session,
				struct avrcp_browsing_header *pdu)
{
	struct avrcp_player *player = target_get_player(session);
	struct get_total_number_of_items_rsp *rsp;

	rsp = (void *)pdu->params;
	rsp->status = AVRCP_STATUS_SUCCESS;
	rsp->uid_counter = htons(player_get_uid_counter(player));
	rsp->num_items = htonl(g_slist_length(session->server->players));
	pdu->param_len = htons(sizeof(*rsp));
}