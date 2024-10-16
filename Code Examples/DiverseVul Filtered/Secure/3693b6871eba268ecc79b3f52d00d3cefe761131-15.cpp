void Server::handleCommand_ModChannelMsg(NetworkPacket *pkt)
{
	std::string channel_name, channel_msg;
	*pkt >> channel_name >> channel_msg;

	session_t peer_id = pkt->getPeerId();
	verbosestream << "Mod channel message received from peer " << peer_id <<
		" on channel " << channel_name << " message: " << channel_msg <<
		std::endl;

	// If mod channels are not enabled, discard message
	if (!g_settings->getBool("enable_mod_channels")) {
		return;
	}

	// If channel not registered, signal it and ignore message
	if (!m_modchannel_mgr->channelRegistered(channel_name)) {
		NetworkPacket resp_pkt(TOCLIENT_MODCHANNEL_SIGNAL,
			1 + 2 + channel_name.size(), peer_id);
		resp_pkt << (u8)MODCHANNEL_SIGNAL_CHANNEL_NOT_REGISTERED << channel_name;
		Send(&resp_pkt);
		return;
	}

	// @TODO: filter, rate limit

	broadcastModChannelMessage(channel_name, channel_msg, peer_id);
}