void Server::handleCommand_SrpBytesA(NetworkPacket* pkt)
{
	session_t peer_id = pkt->getPeerId();
	RemoteClient *client = getClient(peer_id, CS_Invalid);
	ClientState cstate = client->getState();

	bool wantSudo = (cstate == CS_Active);

	if (!((cstate == CS_HelloSent) || (cstate == CS_Active))) {
		actionstream << "Server: got SRP _A packet in wrong state " << cstate <<
			" from " << getPeerAddress(peer_id).serializeString() <<
			". Ignoring." << std::endl;
		return;
	}

	if (client->chosen_mech != AUTH_MECHANISM_NONE) {
		actionstream << "Server: got SRP _A packet, while auth is already "
			"going on with mech " << client->chosen_mech << " from " <<
			getPeerAddress(peer_id).serializeString() <<
			" (wantSudo=" << wantSudo << "). Ignoring." << std::endl;
		if (wantSudo) {
			DenySudoAccess(peer_id);
			return;
		}

		DenyAccess(peer_id, SERVER_ACCESSDENIED_UNEXPECTED_DATA);
		return;
	}

	std::string bytes_A;
	u8 based_on;
	*pkt >> bytes_A >> based_on;

	infostream << "Server: TOSERVER_SRP_BYTES_A received with "
		<< "based_on=" << int(based_on) << " and len_A="
		<< bytes_A.length() << "." << std::endl;

	AuthMechanism chosen = (based_on == 0) ?
		AUTH_MECHANISM_LEGACY_PASSWORD : AUTH_MECHANISM_SRP;

	if (wantSudo) {
		if (!client->isSudoMechAllowed(chosen)) {
			actionstream << "Server: Player \"" << client->getName() <<
				"\" at " << getPeerAddress(peer_id).serializeString() <<
				" tried to change password using unallowed mech " << chosen <<
				"." << std::endl;
			DenySudoAccess(peer_id);
			return;
		}
	} else {
		if (!client->isMechAllowed(chosen)) {
			actionstream << "Server: Client tried to authenticate from " <<
				getPeerAddress(peer_id).serializeString() <<
				" using unallowed mech " << chosen << "." << std::endl;
			DenyAccess(peer_id, SERVER_ACCESSDENIED_UNEXPECTED_DATA);
			return;
		}
	}

	client->chosen_mech = chosen;

	std::string salt;
	std::string verifier;

	if (based_on == 0) {

		generate_srp_verifier_and_salt(client->getName(), client->enc_pwd,
			&verifier, &salt);
	} else if (!decode_srp_verifier_and_salt(client->enc_pwd, &verifier, &salt)) {
		// Non-base64 errors should have been catched in the init handler
		actionstream << "Server: User " << client->getName() <<
			" tried to log in, but srp verifier field was invalid (most likely "
			"invalid base64)." << std::endl;
		DenyAccess(peer_id, SERVER_ACCESSDENIED_SERVER_FAIL);
		return;
	}

	char *bytes_B = 0;
	size_t len_B = 0;

	client->auth_data = srp_verifier_new(SRP_SHA256, SRP_NG_2048,
		client->getName().c_str(),
		(const unsigned char *) salt.c_str(), salt.size(),
		(const unsigned char *) verifier.c_str(), verifier.size(),
		(const unsigned char *) bytes_A.c_str(), bytes_A.size(),
		NULL, 0,
		(unsigned char **) &bytes_B, &len_B, NULL, NULL);

	if (!bytes_B) {
		actionstream << "Server: User " << client->getName()
			<< " tried to log in, SRP-6a safety check violated in _A handler."
			<< std::endl;
		if (wantSudo) {
			DenySudoAccess(peer_id);
			return;
		}

		DenyAccess(peer_id, SERVER_ACCESSDENIED_UNEXPECTED_DATA);
		return;
	}

	NetworkPacket resp_pkt(TOCLIENT_SRP_BYTES_S_B, 0, peer_id);
	resp_pkt << salt << std::string(bytes_B, len_B);
	Send(&resp_pkt);
}