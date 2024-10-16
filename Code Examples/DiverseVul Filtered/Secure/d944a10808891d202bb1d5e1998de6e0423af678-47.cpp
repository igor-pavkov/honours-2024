bool CConnectionTransportUDP::BConnect( const netadr_t &netadrRemote, SteamDatagramErrMsg &errMsg )
{

	// Create an actual OS socket.  We'll bind it to talk only to this host.
	// (Note: we might not actually "bind" it at the OS layer, but from our perpsective
	// it is bound.)
	//
	// For now we're just assuming each connection will gets its own socket,
	// on an ephemeral port.  Later we could add a setting to enable
	// sharing of the socket or binding to a particular local address.
	Assert( !m_pSocket );
	m_pSocket = OpenUDPSocketBoundToHost( netadrRemote, CRecvPacketCallback( PacketReceived, this ), errMsg );
	if ( !m_pSocket )
		return false;
	return true;
}