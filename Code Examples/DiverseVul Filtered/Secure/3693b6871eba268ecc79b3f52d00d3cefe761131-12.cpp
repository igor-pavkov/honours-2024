bool Server::checkInteractDistance(RemotePlayer *player, const f32 d, const std::string &what)
{
	ItemStack selected_item, hand_item;
	player->getWieldedItem(&selected_item, &hand_item);
	f32 max_d = BS * getToolRange(selected_item.getDefinition(m_itemdef),
			hand_item.getDefinition(m_itemdef));

	// Cube diagonal * 1.5 for maximal supported node extents:
	// sqrt(3) * 1.5 ≅ 2.6
	if (d > max_d + 2.6f * BS) {
		actionstream << "Player " << player->getName()
				<< " tried to access " << what
				<< " from too far: "
				<< "d=" << d << ", max_d=" << max_d
				<< "; ignoring." << std::endl;
		// Call callbacks
		m_script->on_cheat(player->getPlayerSAO(), "interacted_too_far");
		return false;
	}
	return true;
}