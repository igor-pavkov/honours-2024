void cli_cm_set_credentials(struct user_auth_info *auth_info)
{
	SAFE_FREE(cm_creds.username);
	cm_creds.username = SMB_STRDUP(get_cmdline_auth_info_username(
					       auth_info));

	if (get_cmdline_auth_info_got_pass(auth_info)) {
		cm_set_password(get_cmdline_auth_info_password(auth_info));
	}

	cm_creds.use_kerberos = get_cmdline_auth_info_use_kerberos(auth_info);
	cm_creds.fallback_after_kerberos = false;
	cm_creds.signing_state = get_cmdline_auth_info_signing_state(auth_info);
}