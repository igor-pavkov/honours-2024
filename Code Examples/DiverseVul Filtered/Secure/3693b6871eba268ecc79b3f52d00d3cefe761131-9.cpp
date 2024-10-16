void Server::handleCommand_FirstSrp(NetworkPacket* pkt)
{
	session_t peer_id = pkt->getPeerId();
	RemoteClient *client = getClient(peer_id, CS_Invalid);
	ClientState cstate = client->getState();

	std::string playername = client->getName();

	std::string salt;
	std::string verification_key;

	std::string addr_s = getPeerAddress(peer_id).serializeString();
	u8 is_empty;

	*pkt >> salt >> verification_key >> is_empty;

	verbosestream << "Server: Got TOSERVER_FIRST_SRP from " << addr_s
		<< ", with is_empty=" << (is_empty == 1) << std::endl;

	// Either this packet is sent because the user is new or to change the password
	if (cstate == CS_HelloSent) {
		if (!client->isMechAllowed(AUTH_MECHANISM_FIRST_SRP)) {
			actionstream << "Server: Client from " << addr_s
					<< " tried to set password without being "
					<< "authenticated, or the username being new." << std::endl;
			DenyAccess(peer_id, SERVER_ACCESSDENIED_UNEXPECTED_DATA);
			return;
		}

		if (!isSingleplayer() &&
				g_settings->getBool("disallow_empty_password") &&
				is_empty == 1) {
			actionstream << "Server: " << playername
					<< " supplied empty password from " << addr_s << std::endl;
			DenyAccess(peer_id, SERVER_ACCESSDENIED_EMPTY_PASSWORD);
			return;
		}

		std::string initial_ver_key;

		initial_ver_key = encode_srp_verifier(verification_key, salt);
		m_script->createAuth(playername, initial_ver_key);
		m_script->on_authplayer(playername, addr_s, true);

		acceptAuth(peer_id, false);
	} else {
		if (cstate < CS_SudoMode) {
			infostream << "Server::ProcessData(): Ignoring TOSERVER_FIRST_SRP from "
					<< addr_s << ": " << "Client has wrong state " << cstate << "."
					<< std::endl;
			return;
		}
		m_clients.event(peer_id, CSE_SudoLeave);
		std::string pw_db_field = encode_srp_verifier(verification_key, salt);
		bool success = m_script->setPassword(playername, pw_db_field);
		if (success) {
			actionstream << playername << " changes password" << std::endl;
			SendChatMessage(peer_id, ChatMessage(CHATMESSAGE_TYPE_SYSTEM,
				L"Password change successful."));
		} else {
			actionstream << playername <<
				" tries to change password but it fails" << std::endl;
			SendChatMessage(peer_id, ChatMessage(CHATMESSAGE_TYPE_SYSTEM,
				L"Password change failed or unavailable."));
		}
	}
}