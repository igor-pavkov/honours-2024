static const char *ccid3_tx_state_name(enum ccid3_hc_tx_states state)
{
	static const char *const ccid3_state_names[] = {
	[TFRC_SSTATE_NO_SENT]  = "NO_SENT",
	[TFRC_SSTATE_NO_FBACK] = "NO_FBACK",
	[TFRC_SSTATE_FBACK]    = "FBACK",
	};

	return ccid3_state_names[state];
}