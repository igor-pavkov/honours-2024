struct cli_state *get_ipc_connect_master_ip_bcast(TALLOC_CTX *ctx,
					const struct user_auth_info *user_info,
					char **pp_workgroup_out)
{
	struct sockaddr_storage *ip_list;
	struct cli_state *cli;
	int i, count;
	NTSTATUS status;

	*pp_workgroup_out = NULL;

        DEBUG(99, ("Do broadcast lookup for workgroups on local network\n"));

        /* Go looking for workgroups by broadcasting on the local network */

	status = name_resolve_bcast(MSBROWSE, 1, talloc_tos(),
				    &ip_list, &count);
        if (!NT_STATUS_IS_OK(status)) {
                DEBUG(99, ("No master browsers responded: %s\n",
			   nt_errstr(status)));
                return NULL;
        }

	for (i = 0; i < count; i++) {
		char addr[INET6_ADDRSTRLEN];
		print_sockaddr(addr, sizeof(addr), &ip_list[i]);
		DEBUG(99, ("Found master browser %s\n", addr));

		cli = get_ipc_connect_master_ip(ctx, &ip_list[i],
				user_info, pp_workgroup_out);
		if (cli)
			return(cli);
	}

	return NULL;
}