void irc_cap_init (void)
{
	signal_add_first("event cap", (SIGNAL_FUNC) event_cap);
	signal_add_first("event 410", (SIGNAL_FUNC) event_invalid_cap);
}