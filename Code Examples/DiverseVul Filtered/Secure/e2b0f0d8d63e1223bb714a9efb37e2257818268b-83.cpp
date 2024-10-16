int avrcp_set_volume(struct btd_device *dev, int8_t volume, bool notify)
{
	struct avrcp_server *server;
	struct avrcp *session;
	uint8_t buf[AVRCP_HEADER_LENGTH + 1];
	struct avrcp_header *pdu = (void *) buf;

	if (volume < 0)
		return -EINVAL;

	server = find_server(servers, device_get_adapter(dev));
	if (server == NULL)
		return -EINVAL;

	session = find_session(server->sessions, dev);
	if (session == NULL)
		return -ENOTCONN;

	if (notify) {
		if (!session->target)
			return -ENOTSUP;
		return avrcp_event(session, AVRCP_EVENT_VOLUME_CHANGED,
								&volume);
	}

	if (!session->controller && !avrcp_event_registered(session,
					AVRCP_EVENT_VOLUME_CHANGED))
		return -ENOTSUP;

	memset(buf, 0, sizeof(buf));

	set_company_id(pdu->company_id, IEEEID_BTSIG);

	pdu->pdu_id = AVRCP_SET_ABSOLUTE_VOLUME;
	pdu->params[0] = volume;
	pdu->params_len = htons(1);

	return avctp_send_vendordep_req(session->conn,
					AVC_CTYPE_CONTROL, AVC_SUBUNIT_PANEL,
					buf, sizeof(buf),
					avrcp_handle_set_volume, session);
}