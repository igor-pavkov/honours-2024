bool CConnectionTransportUDP::BAccept( CSharedSocket *pSharedSock, const netadr_t &netadrRemote, SteamDatagramErrMsg &errMsg )
{
	// Get an interface that is bound to talk to this address
	m_pSocket = pSharedSock->AddRemoteHost( netadrRemote, CRecvPacketCallback( PacketReceived, this ) );
	if ( !m_pSocket )
	{
		// This is really weird and shouldn't happen
		V_strcpy_safe( errMsg, "Unable to create a bound socket on the shared socket." );
		return false;
	}

	return true;
}