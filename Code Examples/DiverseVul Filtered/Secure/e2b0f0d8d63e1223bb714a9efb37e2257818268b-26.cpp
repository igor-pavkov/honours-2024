static bool handle_passthrough(struct avctp *conn, uint8_t op, bool pressed,
							void *user_data)
{
	struct avrcp *session = user_data;
	const struct passthrough_handler *handler;

	for (handler = session->passthrough_handlers; handler->func;
								handler++) {
		if (handler->op == op)
			break;
	}

	if (handler->func == NULL)
		return false;

	/* Do not trigger handler on release */
	if (!pressed)
		return true;

	return handler->func(session);
}