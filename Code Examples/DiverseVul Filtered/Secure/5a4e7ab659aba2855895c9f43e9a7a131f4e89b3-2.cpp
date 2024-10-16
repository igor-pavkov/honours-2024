void irc_cap_deinit (void)
{
	signal_remove("event cap", (SIGNAL_FUNC) event_cap);
	signal_remove("event 410", (SIGNAL_FUNC) event_invalid_cap);
}