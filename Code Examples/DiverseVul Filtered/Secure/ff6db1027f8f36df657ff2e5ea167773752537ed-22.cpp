bool bgp_notify_received_hard_reset(struct peer *peer, uint8_t code,
				    uint8_t subcode)
{
	/* When the "N" bit has been exchanged, a Hard Reset message is used to
	 * indicate to the peer that the session is to be fully terminated.
	 */
	if (!bgp_has_graceful_restart_notification(peer))
		return false;

	if (code == BGP_NOTIFY_CEASE && subcode == BGP_NOTIFY_CEASE_HARD_RESET)
		return true;

	return false;
}