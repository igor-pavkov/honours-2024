static void avrcp_connect_browsing(struct avrcp *session)
{
	/* Immediately connect browsing channel if initiator otherwise delay
	 * it to avoid possible collisions
	 */
	if (avctp_is_initiator(session->conn)) {
		avctp_connect_browsing(session->conn);
		return;
	}

	if (session->browsing_timer > 0)
		return;

	session->browsing_timer = timeout_add_seconds(AVRCP_BROWSING_TIMEOUT,
							connect_browsing,
							session, NULL);
}