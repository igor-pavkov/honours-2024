void CConnectionTransportUDPBase::Received_NoConnection( const CMsgSteamSockets_UDP_NoConnection &msg, SteamNetworkingMicroseconds usecNow )
{
	// Make sure it's an ack of something we would have sent
	if ( msg.to_connection_id() != ConnectionIDLocal() || msg.from_connection_id() != m_connection.m_unConnectionIDRemote )
	{
		ReportBadUDPPacketFromConnectionPeer( "NoConnection", "Old/incorrect connection ID.  Message is for a stale connection, or is spoofed.  Ignoring." );
		return;
	}

	// Generic connection code will take it from here.
	// Closure failure code will only be used if this is news.
	// If we closed the connection (the usual case), it
	// will not be used.
	m_connection.ConnectionState_ClosedByPeer( k_ESteamNetConnectionEnd_Misc_PeerSentNoConnection, "Received unexpected 'no connection' from peer");
}