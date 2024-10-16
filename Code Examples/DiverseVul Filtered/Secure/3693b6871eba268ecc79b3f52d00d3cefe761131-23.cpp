void Server::handleCommand_Damage(NetworkPacket* pkt)
{
	u16 damage;

	*pkt >> damage;

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

	if (!playersao->isImmortal()) {
		if (playersao->isDead()) {
			verbosestream << "Server::ProcessData(): Info: "
				"Ignoring damage as player " << player->getName()
				<< " is already dead." << std::endl;
			return;
		}

		actionstream << player->getName() << " damaged by "
				<< (int)damage << " hp at " << PP(playersao->getBasePosition() / BS)
				<< std::endl;

		PlayerHPChangeReason reason(PlayerHPChangeReason::FALL);
		playersao->setHP((s32)playersao->getHP() - (s32)damage, reason);
		SendPlayerHPOrDie(playersao, reason);
	}
}