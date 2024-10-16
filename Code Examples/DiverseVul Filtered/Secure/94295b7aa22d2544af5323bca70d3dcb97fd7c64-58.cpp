static uint32_t cli_session_setup_capabilities(struct cli_state *cli,
					       uint32_t sesssetup_capabilities)
{
	uint32_t client_capabilities = smb1cli_conn_capabilities(cli->conn);

	/*
	 * We only send capabilities based on the mask for:
	 * - client only flags
	 * - flags used in both directions
	 *
	 * We do not echo the server only flags, except some legacy flags.
	 *
	 * SMB_CAP_LEGACY_CLIENT_MASK contains CAP_LARGE_READX and
	 * CAP_LARGE_WRITEX in order to allow us to do large reads
	 * against old Samba releases (<= 3.6.x).
	 */
	client_capabilities &= (SMB_CAP_BOTH_MASK | SMB_CAP_LEGACY_CLIENT_MASK);

	/*
	 * Session Setup specific flags CAP_DYNAMIC_REAUTH
	 * and CAP_EXTENDED_SECURITY are passed by the caller.
	 * We need that in order to do guest logins even if
	 * CAP_EXTENDED_SECURITY is negotiated.
	 */
	client_capabilities &= ~(CAP_DYNAMIC_REAUTH|CAP_EXTENDED_SECURITY);
	sesssetup_capabilities &= (CAP_DYNAMIC_REAUTH|CAP_EXTENDED_SECURITY);
	client_capabilities |= sesssetup_capabilities;

	return client_capabilities;
}