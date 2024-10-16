void Server::handleCommand_InventoryFields(NetworkPacket* pkt)
{
	std::string client_formspec_name;
	u16 num;

	*pkt >> client_formspec_name >> num;

	StringMap fields;
	for (u16 k = 0; k < num; k++) {
		std::string fieldname;
		*pkt >> fieldname;
		fields[fieldname] = pkt->readLongString();
	}

	session_t peer_id = pkt->getPeerId();
	RemotePlayer *player = m_env->getPlayer(peer_id);

	if (player == NULL) {
		errorstream <<
			"Server::ProcessData(): Canceling: No player for peer_id=" <<
			peer_id << " disconnecting peer!" << std::endl;
		DisconnectPeer(peer_id);
		return;
	}

	PlayerSAO *playersao = player->getPlayerSAO();
	if (playersao == NULL) {
		errorstream <<
			"Server::ProcessData(): Canceling: No player object for peer_id=" <<
			peer_id << " disconnecting peer!" << std::endl;
		DisconnectPeer(peer_id);
		return;
	}

	if (client_formspec_name.empty()) { // pass through inventory submits
		m_script->on_playerReceiveFields(playersao, client_formspec_name, fields);
		return;
	}

	// verify that we displayed the formspec to the user
	const auto peer_state_iterator = m_formspec_state_data.find(peer_id);
	if (peer_state_iterator != m_formspec_state_data.end()) {
		const std::string &server_formspec_name = peer_state_iterator->second;
		if (client_formspec_name == server_formspec_name) {
			auto it = fields.find("quit");
			if (it != fields.end() && it->second == "true")
				m_formspec_state_data.erase(peer_state_iterator);

			m_script->on_playerReceiveFields(playersao, client_formspec_name, fields);
			return;
		}
		actionstream << "'" << player->getName()
			<< "' submitted formspec ('" << client_formspec_name
			<< "') but the name of the formspec doesn't match the"
			" expected name ('" << server_formspec_name << "')";

	} else {
		actionstream << "'" << player->getName()
			<< "' submitted formspec ('" << client_formspec_name
			<< "') but server hasn't sent formspec to client";
	}
	actionstream << ", possible exploitation attempt" << std::endl;
}