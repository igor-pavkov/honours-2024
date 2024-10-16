static int avrcp_event(struct avrcp *session, uint8_t id, const void *data)
{
	uint8_t buf[AVRCP_HEADER_LENGTH + 2];
	struct avrcp_header *pdu = (void *) buf;
	uint8_t code;
	uint16_t size;
	int err;

	/* Verify that the event is registered */
	if (!(session->registered_events & (1 << id)))
		return -ENOENT;

	memset(buf, 0, sizeof(buf));

	set_company_id(pdu->company_id, IEEEID_BTSIG);
	pdu->pdu_id = AVRCP_REGISTER_NOTIFICATION;
	code = AVC_CTYPE_CHANGED;
	pdu->params[0] = id;

	DBG("id=%u", id);

	switch (id) {
	case AVRCP_EVENT_VOLUME_CHANGED:
		size = 2;
		memcpy(&pdu->params[1], data, sizeof(uint8_t));
		break;
	default:
		error("Unknown event %u", id);
		return -EINVAL;
	}

	pdu->params_len = htons(size);

	err = avctp_send_vendordep(session->conn,
					session->transaction_events[id],
					code, AVC_SUBUNIT_PANEL,
					buf, size + AVRCP_HEADER_LENGTH);
	if (err < 0)
		return err;

	/* Unregister event as per AVRCP 1.3 spec, section 5.4.2 */
	session->registered_events ^= 1 << id;

	return err;
}