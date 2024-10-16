void Server::handleCommand_NodeMetaFields(NetworkPacket* pkt)
{
	v3s16 p;
	std::string formname;
	u16 num;

	*pkt >> p >> formname >> num;

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

	// If something goes wrong, this player is to blame
	RollbackScopeActor rollback_scope(m_rollback,
			std::string("player:")+player->getName());

	// Check the target node for rollback data; leave others unnoticed
	RollbackNode rn_old(&m_env->getMap(), p, this);

	m_script->node_on_receive_fields(p, formname, fields, playersao);

	// Report rollback data
	RollbackNode rn_new(&m_env->getMap(), p, this);
	if (rollback() && rn_new != rn_old) {
		RollbackAction action;
		action.setSetNode(p, rn_old, rn_new);
		rollback()->reportAction(action);
	}
}