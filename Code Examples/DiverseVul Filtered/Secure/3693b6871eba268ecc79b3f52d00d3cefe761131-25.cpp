void Server::handleCommand_SrpBytesM(NetworkPacket* pkt)
{
	session_t peer_id = pkt->getPeerId();
	RemoteClient *client = getClient(peer_id, CS_Invalid);
	ClientState cstate = client->getState();
	std::string addr_s = getPeerAddress(pkt->getPeerId()).serializeString();
	std::string playername = client->getName();

	bool wantSudo = (cstate == CS_Active);

	verbosestream << "Server: Received TOCLIENT_SRP_BYTES_M." << std::endl;

	if (!((cstate == CS_HelloSent) || (cstate == CS_Active))) {
		actionstream << "Server: got SRP _M packet in wrong state "
			<< cstate << " from " << addr_s
			<< ". Ignoring." << std::endl;
		return;
	}

	if (client->chosen_mech != AUTH_MECHANISM_SRP &&
			client->chosen_mech != AUTH_MECHANISM_LEGACY_PASSWORD) {
		actionstream << "Server: got SRP _M packet, while auth"
			<< "is going on with mech " << client->chosen_mech << " from "
			<< addr_s << " (wantSudo=" << wantSudo << "). Denying." << std::endl;
		if (wantSudo) {
			DenySudoAccess(peer_id);
			return;
		}

		DenyAccess(peer_id, SERVER_ACCESSDENIED_UNEXPECTED_DATA);
		return;
	}

	std::string bytes_M;
	*pkt >> bytes_M;

	if (srp_verifier_get_session_key_length((SRPVerifier *) client->auth_data)
			!= bytes_M.size()) {
		actionstream << "Server: User " << playername << " at " << addr_s
			<< " sent bytes_M with invalid length " << bytes_M.size() << std::endl;
		DenyAccess(peer_id, SERVER_ACCESSDENIED_UNEXPECTED_DATA);
		return;
	}

	unsigned char *bytes_HAMK = 0;

	srp_verifier_verify_session((SRPVerifier *) client->auth_data,
		(unsigned char *)bytes_M.c_str(), &bytes_HAMK);

	if (!bytes_HAMK) {
		if (wantSudo) {
			actionstream << "Server: User " << playername << " at " << addr_s
				<< " tried to change their password, but supplied wrong"
				<< " (SRP) password for authentication." << std::endl;
			DenySudoAccess(peer_id);
			return;
		}

		actionstream << "Server: User " << playername << " at " << addr_s
			<< " supplied wrong password (auth mechanism: SRP)." << std::endl;
		m_script->on_authplayer(playername, addr_s, false);
		DenyAccess(peer_id, SERVER_ACCESSDENIED_WRONG_PASSWORD);
		return;
	}

	if (client->create_player_on_auth_success) {
		m_script->createAuth(playername, client->enc_pwd);

		std::string checkpwd; // not used, but needed for passing something
		if (!m_script->getAuth(playername, &checkpwd, NULL)) {
			actionstream << "Server: " << playername <<
				" cannot be authenticated (auth handler does not work?)" <<
				std::endl;
			DenyAccess(peer_id, SERVER_ACCESSDENIED_SERVER_FAIL);
			return;
		}
		client->create_player_on_auth_success = false;
	}

	m_script->on_authplayer(playername, addr_s, true);
	acceptAuth(peer_id, wantSudo);
}