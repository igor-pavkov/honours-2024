void CSteamNetworkListenSocketDirectUDP::ReceivedFromUnknownHost( const void *pvPkt, int cbPkt, const netadr_t &adrFrom, CSteamNetworkListenSocketDirectUDP *pSock )
{
	const uint8 *pPkt = static_cast<const uint8 *>( pvPkt );

	SteamNetworkingMicroseconds usecNow = SteamNetworkingSockets_GetLocalTimestamp();

	if ( cbPkt < 5 )
	{
		ReportBadPacket( "packet", "%d byte packet is too small", cbPkt );
		return;
	}

	if ( *pPkt & 0x80 )
	{
		if ( *(uint32*)pPkt == 0xffffffff )
		{
			// Source engine connectionless packet (LAN discovery, etc).
			// Just ignore it, and don't even spew.
		}
		else
		{
			// A stray data packet.  Just ignore it.
			//
			// When clients are able to actually establish a connection, after that connection
			// is over we will use the FinWait state to close down the connection gracefully.
			// But since we don't have that connection in our table anymore, either this guy
			// never had a connection, or else we believe he knows that the connection was closed,
			// or the FinWait state has timed out.
			ReportBadPacket( "Data", "Stray data packet from host with no connection.  Ignoring." );
		}
	}
	else if ( *pPkt == k_ESteamNetworkingUDPMsg_ChallengeRequest )
	{
		ParsePaddedPacket( pvPkt, cbPkt, CMsgSteamSockets_UDP_ChallengeRequest, msg )
		pSock->Received_ChallengeRequest( msg, adrFrom, usecNow );
	}
	else if ( *pPkt == k_ESteamNetworkingUDPMsg_ConnectRequest )
	{
		ParseProtobufBody( pPkt+1, cbPkt-1, CMsgSteamSockets_UDP_ConnectRequest, msg )
		pSock->Received_ConnectRequest( msg, adrFrom, cbPkt, usecNow );
	}
	else if ( *pPkt == k_ESteamNetworkingUDPMsg_ConnectionClosed )
	{
		ParsePaddedPacket( pvPkt, cbPkt, CMsgSteamSockets_UDP_ConnectionClosed, msg )
		pSock->Received_ConnectionClosed( msg, adrFrom, usecNow );
	}
	else if ( *pPkt == k_ESteamNetworkingUDPMsg_NoConnection )
	{
		// They don't think there's a connection on this address.
		// We agree -- connection ID doesn't matter.  Nothing else to do.
	}
	else
	{
		// Any other lead byte is bogus
		//
		// Note in particular that these packet types should be ignored:
		//
		// k_ESteamNetworkingUDPMsg_ChallengeReply
		// k_ESteamNetworkingUDPMsg_ConnectOK
		//
		// We are not initiating connections, so we shouldn't ever get
		// those sorts of replies.

		ReportBadPacket( "packet", "Invalid lead byte 0x%02x", *pPkt );
	}
}