struct cli_state *get_ipc_connect(char *server,
				struct sockaddr_storage *server_ss,
				const struct user_auth_info *user_info)
{
        struct cli_state *cli;
	NTSTATUS nt_status;
	uint32_t flags = CLI_FULL_CONNECTION_ANONYMOUS_FALLBACK;

	if (user_info->use_kerberos) {
		flags |= CLI_FULL_CONNECTION_USE_KERBEROS;
	}

	nt_status = cli_full_connection(&cli, NULL, server, server_ss, 0, "IPC$", "IPC", 
					user_info->username ? user_info->username : "",
					lp_workgroup(),
					user_info->password ? user_info->password : "",
					flags,
					SMB_SIGNING_DEFAULT);

	if (NT_STATUS_IS_OK(nt_status)) {
		return cli;
	} else if (is_ipaddress(server)) {
	    /* windows 9* needs a correct NMB name for connections */
	    fstring remote_name;

	    if (name_status_find("*", 0, 0, server_ss, remote_name)) {
		cli = get_ipc_connect(remote_name, server_ss, user_info);
		if (cli)
		    return cli;
	    }
	}
	return NULL;
}