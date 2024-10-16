static void session_init_control(struct avrcp *session)
{
	session->passthrough_id = avctp_register_passthrough_handler(
							session->conn,
							handle_passthrough,
							session);
	session->passthrough_handlers = passthrough_handlers;
	session->control_id = avctp_register_pdu_handler(session->conn,
							AVC_OP_VENDORDEP,
							handle_vendordep_pdu,
							session);
	session->control_handlers = control_handlers;

	if (btd_device_get_service(session->dev, AVRCP_TARGET_UUID) != NULL)
		controller_init(session);

	if (btd_device_get_service(session->dev, AVRCP_REMOTE_UUID) != NULL)
		target_init(session);
}