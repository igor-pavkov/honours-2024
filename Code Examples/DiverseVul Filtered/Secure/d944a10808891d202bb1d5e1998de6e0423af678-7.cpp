bool CConnectionTransportUDP::CreateLoopbackPair( CConnectionTransportUDP *pTransport[2] )
{
	IBoundUDPSocket *sock[2];
	SteamNetworkingErrMsg errMsg;
	if ( !CreateBoundSocketPair(
		CRecvPacketCallback( PacketReceived, pTransport[0] ),
		CRecvPacketCallback( PacketReceived, pTransport[1] ), sock, errMsg ) )
	{
		// Assert, this really should only fail if we have some sort of bug
		AssertMsg1( false, "Failed to create UDP socket pair.  %s", errMsg );
		return false;
	}

	pTransport[0]->m_pSocket = sock[0];
	pTransport[1]->m_pSocket = sock[1];

	return true;
}