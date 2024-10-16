NTSTATUS cli_cm_open(TALLOC_CTX *ctx,
				struct cli_state *referring_cli,
				const char *server,
				const char *share,
				const struct user_auth_info *auth_info,
				bool show_hdr,
				bool force_encrypt,
				int max_protocol,
				int port,
				int name_type,
				struct cli_state **pcli)
{
	/* Try to reuse an existing connection in this list. */
	struct cli_state *c = cli_cm_find(referring_cli, server, share);
	NTSTATUS status;

	if (c) {
		*pcli = c;
		return NT_STATUS_OK;
	}

	if (auth_info == NULL) {
		/* Can't do a new connection
		 * without auth info. */
		d_printf("cli_cm_open() Unable to open connection [\\%s\\%s] "
			"without auth info\n",
			server, share );
		return NT_STATUS_INVALID_PARAMETER;
	}

	status = cli_cm_connect(ctx,
				referring_cli,
				server,
				share,
				auth_info,
				show_hdr,
				force_encrypt,
				max_protocol,
				port,
				name_type,
				&c);
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}
	*pcli = c;
	return NT_STATUS_OK;
}