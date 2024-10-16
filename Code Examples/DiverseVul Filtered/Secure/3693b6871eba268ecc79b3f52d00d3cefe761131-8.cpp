void Server::handleCommand_Respawn(NetworkPacket* pkt)
{
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
	assert(playersao);

	if (!playersao->isDead())
		return;

	RespawnPlayer(peer_id);

	actionstream << player->getName() << " respawns at "
			<< PP(playersao->getBasePosition() / BS) << std::endl;

	// ActiveObject is added to environment in AsyncRunStep after
	// the previous addition has been successfully removed
}