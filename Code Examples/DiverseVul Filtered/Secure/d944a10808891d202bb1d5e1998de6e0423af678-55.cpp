void CConnectionTransportUDP::Received_ChallengeOrConnectRequest( const char *pszDebugPacketType, uint32 unPacketConnectionID, SteamNetworkingMicroseconds usecNow )
{
	// If wrong connection ID, then check for sending a generic reply and bail
	if ( unPacketConnectionID != m_connection.m_unConnectionIDRemote )
	{
		ReportBadUDPPacketFromConnectionPeer( pszDebugPacketType, "Incorrect connection ID, when we do have a connection for this address.  Could be spoofed, ignoring." );
		// Let's not send a reply in this case
		//if ( BCheckGlobalSpamReplyRateLimit( usecNow ) )
		//	SendNoConnection( unPacketConnectionID );
		return;
	}

	// Check state
	switch ( ConnectionState() )
	{
		case k_ESteamNetworkingConnectionState_Dead:
		case k_ESteamNetworkingConnectionState_None:
		case k_ESteamNetworkingConnectionState_FindingRoute: // not used for raw UDP
		default:
			Assert( false );
			return;

		case k_ESteamNetworkingConnectionState_ClosedByPeer:
		case k_ESteamNetworkingConnectionState_FinWait:
		case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
			SendConnectionClosedOrNoConnection();
			return;

		case k_ESteamNetworkingConnectionState_Connecting:
			// We're waiting on the application.  So we'll just have to ignore.
			break;

		case k_ESteamNetworkingConnectionState_Linger:
		case k_ESteamNetworkingConnectionState_Connected:
			if ( !ListenSocket() )
			{
				// WAT?  We initiated this connection, so why are they requesting to connect?
				ReportBadUDPPacketFromConnectionPeer( pszDebugPacketType, "We are the 'client' who initiated the connection, so 'server' shouldn't be sending us this!" );
				return;
			}

			// This is totally legit and possible.  Our earlier reply might have dropped, and they are re-sending
			SendConnectOK( usecNow );
			return;
	}

}