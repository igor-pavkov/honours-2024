void Server::handleCommand_ChatMessage(NetworkPacket* pkt)
{
	/*
		u16 command
		u16 length
		wstring message
	*/
	u16 len;
	*pkt >> len;

	std::wstring message;
	for (u16 i = 0; i < len; i++) {
		u16 tmp_wchar;
		*pkt >> tmp_wchar;

		message += (wchar_t)tmp_wchar;
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

	// Get player name of this client
	std::string name = player->getName();
	std::wstring wname = narrow_to_wide(name);

	std::wstring answer_to_sender = handleChat(name, wname, message, true, player);
	if (!answer_to_sender.empty()) {
		// Send the answer to sender
		SendChatMessage(peer_id, ChatMessage(CHATMESSAGE_TYPE_NORMAL,
			answer_to_sender, wname));
	}
}