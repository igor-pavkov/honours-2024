static void avrcp_track_changed(struct avrcp *session,
						struct avrcp_header *pdu)
{
	if (session->browsing_id) {
		struct avrcp_player *player = session->controller->player;
		player->uid = get_be64(&pdu->params[1]);
		avrcp_get_item_attributes(session, player->uid);
	} else
		avrcp_get_element_attributes(session);
}