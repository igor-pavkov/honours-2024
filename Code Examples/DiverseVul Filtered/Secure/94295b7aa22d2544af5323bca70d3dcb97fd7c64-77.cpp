struct cli_state *get_ipc_connect_master_ip(TALLOC_CTX *ctx,
				struct sockaddr_storage *mb_ip,
				const struct user_auth_info *user_info,
				char **pp_workgroup_out)
{
	char addr[INET6_ADDRSTRLEN];
        fstring name;
	struct cli_state *cli;
	struct sockaddr_storage server_ss;

	*pp_workgroup_out = NULL;

	print_sockaddr(addr, sizeof(addr), mb_ip);
        DEBUG(99, ("Looking up name of master browser %s\n",
                   addr));

        /*
         * Do a name status query to find out the name of the master browser.
         * We use <01><02>__MSBROWSE__<02>#01 if *#00 fails because a domain
         * master browser will not respond to a wildcard query (or, at least,
         * an NT4 server acting as the domain master browser will not).
         *
         * We might be able to use ONLY the query on MSBROWSE, but that's not
         * yet been tested with all Windows versions, so until it is, leave
         * the original wildcard query as the first choice and fall back to
         * MSBROWSE if the wildcard query fails.
         */
        if (!name_status_find("*", 0, 0x1d, mb_ip, name) &&
            !name_status_find(MSBROWSE, 1, 0x1d, mb_ip, name)) {

                DEBUG(99, ("Could not retrieve name status for %s\n",
                           addr));
                return NULL;
        }

        if (!find_master_ip(name, &server_ss)) {
                DEBUG(99, ("Could not find master ip for %s\n", name));
                return NULL;
        }

	*pp_workgroup_out = talloc_strdup(ctx, name);

	DEBUG(4, ("found master browser %s, %s\n", name, addr));

	print_sockaddr(addr, sizeof(addr), &server_ss);
	cli = get_ipc_connect(addr, &server_ss, user_info);

	return cli;
}