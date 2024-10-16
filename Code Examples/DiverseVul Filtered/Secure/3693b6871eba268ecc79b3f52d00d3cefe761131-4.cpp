void Server::handleCommand_Deprecated(NetworkPacket* pkt)
{
	infostream << "Server: " << toServerCommandTable[pkt->getCommand()].name
		<< " not supported anymore" << std::endl;
}