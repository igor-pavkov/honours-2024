static bool connect_browsing(gpointer user_data)
{
	struct avrcp *session = user_data;

	session->browsing_timer = 0;

	avctp_connect_browsing(session->conn);

	return FALSE;
}