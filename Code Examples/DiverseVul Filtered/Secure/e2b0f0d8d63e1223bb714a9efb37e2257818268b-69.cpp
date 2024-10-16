static struct avrcp *session_create(struct avrcp_server *server,
						struct btd_device *device)
{
	struct avrcp *session;

	session = g_new0(struct avrcp, 1);
	session->server = server;
	session->conn = avctp_connect(device);
	session->dev = device;

	server->sessions = g_slist_append(server->sessions, session);

	return session;
}