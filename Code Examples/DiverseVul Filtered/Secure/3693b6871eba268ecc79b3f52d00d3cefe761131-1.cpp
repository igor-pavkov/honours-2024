void Server::handleCommand_RequestMedia(NetworkPacket* pkt)
{
	std::vector<std::string> tosend;
	u16 numfiles;

	*pkt >> numfiles;

	session_t peer_id = pkt->getPeerId();
	infostream << "Sending " << numfiles << " files to " <<
		getPlayerName(peer_id) << std::endl;
	verbosestream << "TOSERVER_REQUEST_MEDIA: " << std::endl;

	for (u16 i = 0; i < numfiles; i++) {
		std::string name;

		*pkt >> name;

		tosend.push_back(name);
		verbosestream << "TOSERVER_REQUEST_MEDIA: requested file "
				<< name << std::endl;
	}

	sendRequestedMedia(peer_id, tosend);
}