void Server::handleCommand_ModChannelJoin(NetworkPacket *pkt)
{
	std::string channel_name;
	*pkt >> channel_name;

	session_t peer_id = pkt->getPeerId();
	NetworkPacket resp_pkt(TOCLIENT_MODCHANNEL_SIGNAL,
		1 + 2 + channel_name.size(), peer_id);

	// Send signal to client to notify join succeed or not
	if (g_settings->getBool("enable_mod_channels") &&
			m_modchannel_mgr->joinChannel(channel_name, peer_id)) {
		resp_pkt << (u8) MODCHANNEL_SIGNAL_JOIN_OK;
		infostream << "Peer " << peer_id << " joined channel " <<
			channel_name << std::endl;
	}
	else {
		resp_pkt << (u8)MODCHANNEL_SIGNAL_JOIN_FAILURE;
		infostream << "Peer " << peer_id << " tried to join channel " <<
			channel_name << ", but was already registered." << std::endl;
	}
	resp_pkt << channel_name;
	Send(&resp_pkt);
}