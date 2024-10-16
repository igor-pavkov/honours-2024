void Server::handleCommand_DeletedBlocks(NetworkPacket* pkt)
{
	if (pkt->getSize() < 1)
		return;

	/*
		[0] u16 command
		[2] u8 count
		[3] v3s16 pos_0
		[3+6] v3s16 pos_1
		...
	*/

	u8 count;
	*pkt >> count;

	RemoteClient *client = getClient(pkt->getPeerId());

	if ((s16)pkt->getSize() < 1 + (int)count * 6) {
		throw con::InvalidIncomingDataException
				("DELETEDBLOCKS length is too short");
	}

	for (u16 i = 0; i < count; i++) {
		v3s16 p;
		*pkt >> p;
		client->SetBlockNotSent(p);
	}
}