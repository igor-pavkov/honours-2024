static bool avrcp_event_registered(struct avrcp *session, uint8_t event)
{
	return session->registered_events & (1 << event);
}