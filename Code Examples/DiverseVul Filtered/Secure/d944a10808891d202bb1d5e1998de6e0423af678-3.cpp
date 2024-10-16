void CConnectionTransportUDP::PacketReceived( const void *pvPkt, int cbPkt, const netadr_t &adrFrom, CConnectionTransportUDP *pSelf )
{
	const uint8 *pPkt = static_cast<const uint8 *>( pvPkt );

	SteamNetworkingMicroseconds usecNow = SteamNetworkingSockets_GetLocalTimestamp();

	if ( cbPkt < 5 )
	{
		ReportBadPacket( "packet", "%d byte packet is too small", cbPkt );
		return;
	}

	// Data packet is the most common, check for it first.  Also, does stat tracking.
	if ( *pPkt & 0x80 )
	{
		pSelf->Received_Data( pPkt, cbPkt, usecNow );
		return;
	}

	// Track stats for other packet types.
	pSelf->m_connection.m_statsEndToEnd.TrackRecvPacket( cbPkt, usecNow );

	if ( *pPkt == k_ESteamNetworkingUDPMsg_ChallengeReply )
	{
		ParseProtobufBody( pPkt+1, cbPkt-1, CMsgSteamSockets_UDP_ChallengeReply, msg )
		pSelf->Received_ChallengeReply( msg, usecNow );
	}
	else if ( *pPkt == k_ESteamNetworkingUDPMsg_ConnectOK )
	{
		ParseProtobufBody( pPkt+1, cbPkt-1, CMsgSteamSockets_UDP_ConnectOK, msg );
		pSelf->Received_ConnectOK( msg, usecNow );
	}
	else if ( *pPkt == k_ESteamNetworkingUDPMsg_ConnectionClosed )
	{
		ParsePaddedPacket( pvPkt, cbPkt, CMsgSteamSockets_UDP_ConnectionClosed, msg )
		pSelf->Received_ConnectionClosed( msg, usecNow );
	}
	else if ( *pPkt == k_ESteamNetworkingUDPMsg_NoConnection )
	{
		ParseProtobufBody( pPkt+1, cbPkt-1, CMsgSteamSockets_UDP_NoConnection, msg )
		pSelf->Received_NoConnection( msg, usecNow );
	}
	else if ( *pPkt == k_ESteamNetworkingUDPMsg_ChallengeRequest )
	{
		ParsePaddedPacket( pvPkt, cbPkt, CMsgSteamSockets_UDP_ChallengeRequest, msg )
		pSelf->Received_ChallengeOrConnectRequest( "ChallengeRequest", msg.connection_id(), usecNow );
	}
	else if ( *pPkt == k_ESteamNetworkingUDPMsg_ConnectRequest )
	{
		ParseProtobufBody( pPkt+1, cbPkt-1, CMsgSteamSockets_UDP_ConnectRequest, msg )
		pSelf->Received_ChallengeOrConnectRequest( "ConnectRequest", msg.client_connection_id(), usecNow );
	}
	else
	{
		ReportBadPacket( "packet", "Lead byte 0x%02x not a known message ID", *pPkt );
	}
}