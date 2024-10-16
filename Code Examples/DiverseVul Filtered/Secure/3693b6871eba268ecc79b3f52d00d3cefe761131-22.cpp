void Server::handleCommand_Init(NetworkPacket* pkt)
{

	if(pkt->getSize() < 1)
		return;

	session_t peer_id = pkt->getPeerId();
	RemoteClient *client = getClient(peer_id, CS_Created);

	std::string addr_s;
	try {
		Address address = getPeerAddress(peer_id);
		addr_s = address.serializeString();
	}
	catch (con::PeerNotFoundException &e) {
		/*
		 * no peer for this packet found
		 * most common reason is peer timeout, e.g. peer didn't
		 * respond for some time, your server was overloaded or
		 * things like that.
		 */
		infostream << "Server::ProcessData(): Canceling: peer " << peer_id <<
			" not found" << std::endl;
		return;
	}

	// If net_proto_version is set, this client has already been handled
	if (client->getState() > CS_Created) {
		verbosestream << "Server: Ignoring multiple TOSERVER_INITs from " <<
			addr_s << " (peer_id=" << peer_id << ")" << std::endl;
		return;
	}

	verbosestream << "Server: Got TOSERVER_INIT from " << addr_s <<
		" (peer_id=" << peer_id << ")" << std::endl;

	// Do not allow multiple players in simple singleplayer mode.
	// This isn't a perfect way to do it, but will suffice for now
	if (m_simple_singleplayer_mode && m_clients.getClientIDs().size() > 1) {
		infostream << "Server: Not allowing another client (" << addr_s <<
			") to connect in simple singleplayer mode" << std::endl;
		DenyAccess(peer_id, SERVER_ACCESSDENIED_SINGLEPLAYER);
		return;
	}

	// First byte after command is maximum supported
	// serialization version
	u8 client_max;
	u16 supp_compr_modes;
	u16 min_net_proto_version = 0;
	u16 max_net_proto_version;
	std::string playerName;

	*pkt >> client_max >> supp_compr_modes >> min_net_proto_version
			>> max_net_proto_version >> playerName;

	u8 our_max = SER_FMT_VER_HIGHEST_READ;
	// Use the highest version supported by both
	u8 depl_serial_v = std::min(client_max, our_max);
	// If it's lower than the lowest supported, give up.
	if (depl_serial_v < SER_FMT_VER_LOWEST_READ)
		depl_serial_v = SER_FMT_VER_INVALID;

	if (depl_serial_v == SER_FMT_VER_INVALID) {
		actionstream << "Server: A mismatched client tried to connect from " <<
			addr_s << " ser_fmt_max=" << (int)client_max << std::endl;
		DenyAccess(peer_id, SERVER_ACCESSDENIED_WRONG_VERSION);
		return;
	}

	client->setPendingSerializationVersion(depl_serial_v);

	/*
		Read and check network protocol version
	*/

	u16 net_proto_version = 0;

	// Figure out a working version if it is possible at all
	if (max_net_proto_version >= SERVER_PROTOCOL_VERSION_MIN ||
			min_net_proto_version <= SERVER_PROTOCOL_VERSION_MAX) {
		// If maximum is larger than our maximum, go with our maximum
		if (max_net_proto_version > SERVER_PROTOCOL_VERSION_MAX)
			net_proto_version = SERVER_PROTOCOL_VERSION_MAX;
		// Else go with client's maximum
		else
			net_proto_version = max_net_proto_version;
	}

	verbosestream << "Server: " << addr_s << ": Protocol version: min: "
			<< min_net_proto_version << ", max: " << max_net_proto_version
			<< ", chosen: " << net_proto_version << std::endl;

	client->net_proto_version = net_proto_version;

	if ((g_settings->getBool("strict_protocol_version_checking") &&
			net_proto_version != LATEST_PROTOCOL_VERSION) ||
			net_proto_version < SERVER_PROTOCOL_VERSION_MIN ||
			net_proto_version > SERVER_PROTOCOL_VERSION_MAX) {
		actionstream << "Server: A mismatched client tried to connect from " <<
			addr_s << " proto_max=" << (int)max_net_proto_version << std::endl;
		DenyAccess(peer_id, SERVER_ACCESSDENIED_WRONG_VERSION);
		return;
	}

	/*
		Validate player name
	*/
	const char* playername = playerName.c_str();

	size_t pns = playerName.size();
	if (pns == 0 || pns > PLAYERNAME_SIZE) {
		actionstream << "Server: Player with " <<
			((pns > PLAYERNAME_SIZE) ? "a too long" : "an empty") <<
			" name tried to connect from " << addr_s << std::endl;
		DenyAccess(peer_id, SERVER_ACCESSDENIED_WRONG_NAME);
		return;
	}

	if (!string_allowed(playerName, PLAYERNAME_ALLOWED_CHARS)) {
		actionstream << "Server: Player with an invalid name tried to connect "
			"from " << addr_s << std::endl;
		DenyAccess(peer_id, SERVER_ACCESSDENIED_WRONG_CHARS_IN_NAME);
		return;
	}

	m_clients.setPlayerName(peer_id, playername);
	//TODO (later) case insensitivity

	std::string legacyPlayerNameCasing = playerName;

	if (!isSingleplayer() && strcasecmp(playername, "singleplayer") == 0) {
		actionstream << "Server: Player with the name \"singleplayer\" tried "
			"to connect from " << addr_s << std::endl;
		DenyAccess(peer_id, SERVER_ACCESSDENIED_WRONG_NAME);
		return;
	}

	{
		std::string reason;
		if (m_script->on_prejoinplayer(playername, addr_s, &reason)) {
			actionstream << "Server: Player with the name \"" << playerName <<
				"\" tried to connect from " << addr_s <<
				" but it was disallowed for the following reason: " << reason <<
				std::endl;
			DenyAccess(peer_id, SERVER_ACCESSDENIED_CUSTOM_STRING, reason);
			return;
		}
	}

	infostream << "Server: New connection: \"" << playerName << "\" from " <<
		addr_s << " (peer_id=" << peer_id << ")" << std::endl;

	// Enforce user limit.
	// Don't enforce for users that have some admin right or mod permits it.
	if (m_clients.isUserLimitReached() &&
			playername != g_settings->get("name") &&
			!m_script->can_bypass_userlimit(playername, addr_s)) {
		actionstream << "Server: " << playername << " tried to join from " <<
			addr_s << ", but there are already max_users=" <<
			g_settings->getU16("max_users") << " players." << std::endl;
		DenyAccess(peer_id, SERVER_ACCESSDENIED_TOO_MANY_USERS);
		return;
	}

	/*
		Compose auth methods for answer
	*/
	std::string encpwd; // encrypted Password field for the user
	bool has_auth = m_script->getAuth(playername, &encpwd, NULL);
	u32 auth_mechs = 0;

	client->chosen_mech = AUTH_MECHANISM_NONE;

	if (has_auth) {
		std::vector<std::string> pwd_components = str_split(encpwd, '#');
		if (pwd_components.size() == 4) {
			if (pwd_components[1] == "1") { // 1 means srp
				auth_mechs |= AUTH_MECHANISM_SRP;
				client->enc_pwd = encpwd;
			} else {
				actionstream << "User " << playername << " tried to log in, "
					"but password field was invalid (unknown mechcode)." <<
					std::endl;
				DenyAccess(peer_id, SERVER_ACCESSDENIED_SERVER_FAIL);
				return;
			}
		} else if (base64_is_valid(encpwd)) {
			auth_mechs |= AUTH_MECHANISM_LEGACY_PASSWORD;
			client->enc_pwd = encpwd;
		} else {
			actionstream << "User " << playername << " tried to log in, but "
				"password field was invalid (invalid base64)." << std::endl;
			DenyAccess(peer_id, SERVER_ACCESSDENIED_SERVER_FAIL);
			return;
		}
	} else {
		std::string default_password = g_settings->get("default_password");
		if (default_password.length() == 0) {
			auth_mechs |= AUTH_MECHANISM_FIRST_SRP;
		} else {
			// Take care of default passwords.
			client->enc_pwd = get_encoded_srp_verifier(playerName, default_password);
			auth_mechs |= AUTH_MECHANISM_SRP;
			// Allocate player in db, but only on successful login.
			client->create_player_on_auth_success = true;
		}
	}

	/*
		Answer with a TOCLIENT_HELLO
	*/

	verbosestream << "Sending TOCLIENT_HELLO with auth method field: "
		<< auth_mechs << std::endl;

	NetworkPacket resp_pkt(TOCLIENT_HELLO,
		1 + 4 + legacyPlayerNameCasing.size(), peer_id);

	u16 depl_compress_mode = NETPROTO_COMPRESSION_NONE;
	resp_pkt << depl_serial_v << depl_compress_mode << net_proto_version
		<< auth_mechs << legacyPlayerNameCasing;

	Send(&resp_pkt);

	client->allowed_auth_mechs = auth_mechs;
	client->setDeployedCompressionMode(depl_compress_mode);

	m_clients.event(peer_id, CSE_Hello);
}