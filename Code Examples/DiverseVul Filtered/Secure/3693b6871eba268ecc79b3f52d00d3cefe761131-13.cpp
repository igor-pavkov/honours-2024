void Server::handleCommand_ClientReady(NetworkPacket* pkt)
{
	session_t peer_id = pkt->getPeerId();

	PlayerSAO* playersao = StageTwoClientInit(peer_id);

	if (playersao == NULL) {
		errorstream << "TOSERVER_CLIENT_READY stage 2 client init failed "
			"peer_id=" << peer_id << std::endl;
		DisconnectPeer(peer_id);
		return;
	}


	if (pkt->getSize() < 8) {
		errorstream << "TOSERVER_CLIENT_READY client sent inconsistent data, "
			"disconnecting peer_id: " << peer_id << std::endl;
		DisconnectPeer(peer_id);
		return;
	}

	u8 major_ver, minor_ver, patch_ver, reserved;
	std::string full_ver;
	*pkt >> major_ver >> minor_ver >> patch_ver >> reserved >> full_ver;

	m_clients.setClientVersion(peer_id, major_ver, minor_ver, patch_ver,
		full_ver);

	if (pkt->getRemainingBytes() >= 2)
		*pkt >> playersao->getPlayer()->formspec_version;

	const std::vector<std::string> &players = m_clients.getPlayerNames();
	NetworkPacket list_pkt(TOCLIENT_UPDATE_PLAYER_LIST, 0, peer_id);
	list_pkt << (u8) PLAYER_LIST_INIT << (u16) players.size();
	for (const std::string &player: players) {
		list_pkt <<  player;
	}
	m_clients.send(peer_id, 0, &list_pkt, true);

	NetworkPacket notice_pkt(TOCLIENT_UPDATE_PLAYER_LIST, 0, PEER_ID_INEXISTENT);
	// (u16) 1 + std::string represents a pseudo vector serialization representation
	notice_pkt << (u8) PLAYER_LIST_ADD << (u16) 1 << std::string(playersao->getPlayer()->getName());
	m_clients.sendToAll(&notice_pkt);
	m_clients.event(peer_id, CSE_SetClientReady);

	s64 last_login;
	m_script->getAuth(playersao->getPlayer()->getName(), nullptr, nullptr, &last_login);
	m_script->on_joinplayer(playersao, last_login);

	// Send shutdown timer if shutdown has been scheduled
	if (m_shutdown_state.isTimerRunning()) {
		SendChatMessage(peer_id, m_shutdown_state.getShutdownTimerMessage());
	}
}