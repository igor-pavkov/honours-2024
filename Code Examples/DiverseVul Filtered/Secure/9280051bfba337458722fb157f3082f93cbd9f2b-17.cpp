static NTSTATUS do_map_to_guest(NTSTATUS status,
				auth_serversupplied_info **server_info,
				const char *user, const char *domain)
{
	if (NT_STATUS_EQUAL(status, NT_STATUS_NO_SUCH_USER)) {
		if ((lp_map_to_guest() == MAP_TO_GUEST_ON_BAD_USER) ||
		    (lp_map_to_guest() == MAP_TO_GUEST_ON_BAD_PASSWORD)) {
			DEBUG(3,("No such user %s [%s] - using guest account\n",
				 user, domain));
			status = make_server_info_guest(NULL, server_info);
		}
	}

	if (NT_STATUS_EQUAL(status, NT_STATUS_WRONG_PASSWORD)) {
		if (lp_map_to_guest() == MAP_TO_GUEST_ON_BAD_PASSWORD) {
			DEBUG(3,("Registered username %s for guest access\n",
				user));
			status = make_server_info_guest(NULL, server_info);
		}
	}

	return status;
}