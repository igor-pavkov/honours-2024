void CConnectionTransportUDP::Received_ChallengeReply( const CMsgSteamSockets_UDP_ChallengeReply &msg, SteamNetworkingMicroseconds usecNow )
{
	// We should only be getting this if we are the "client"
	if ( ListenSocket() )
	{
		ReportBadUDPPacketFromConnectionPeer( "ChallengeReply", "Shouldn't be receiving this unless on accepted connections, only connections initiated locally." );
		return;
	}

	// Ignore if we're not trying to connect
	if ( ConnectionState() != k_ESteamNetworkingConnectionState_Connecting )
		return;

	// Check session ID to make sure they aren't spoofing.
	if ( msg.connection_id() != ConnectionIDLocal() )
	{
		ReportBadUDPPacketFromConnectionPeer( "ChallengeReply", "Incorrect connection ID.  Message is stale or could be spoofed, ignoring." );
		return;
	}
	if ( msg.protocol_version() < k_nMinRequiredProtocolVersion )
	{
		m_connection.ConnectionState_ProblemDetectedLocally( k_ESteamNetConnectionEnd_Misc_Generic, "Peer is running old software and needs to be udpated" );
		return;
	}

	// Update ping, if they replied with the timestamp
	if ( msg.has_your_timestamp() )
	{
		SteamNetworkingMicroseconds usecElapsed = usecNow - (SteamNetworkingMicroseconds)msg.your_timestamp();
		if ( usecElapsed < 0 || usecElapsed > 2*k_nMillion )
		{
			SpewWarning( "Ignoring weird timestamp %llu in ChallengeReply, current time is %llu.\n", (unsigned long long)msg.your_timestamp(), usecNow );
		}
		else
		{
			int nPing = (usecElapsed + 500 ) / 1000;
			m_connection.m_statsEndToEnd.m_ping.ReceivedPing( nPing, usecNow );
		}
	}

	// Make sure we have the crypt info that we need
	if ( !m_connection.GetSignedCertLocal().has_cert() || !m_connection.GetSignedCryptLocal().has_info() )
	{
		m_connection.ConnectionState_ProblemDetectedLocally( k_ESteamNetConnectionEnd_Misc_InternalError, "Tried to connect request, but crypt not ready" );
		return;
	}

	// Remember protocol version.  They must send it again in the connect OK, but we have a valid value now,
	// so we might as well save it
	m_connection.m_statsEndToEnd.m_nPeerProtocolVersion = msg.protocol_version();

	// Reply with the challenge data and our cert
	CMsgSteamSockets_UDP_ConnectRequest msgConnectRequest;
	msgConnectRequest.set_client_connection_id( ConnectionIDLocal() );
	msgConnectRequest.set_challenge( msg.challenge() );
	msgConnectRequest.set_my_timestamp( usecNow );
	if ( m_connection.m_statsEndToEnd.m_ping.m_nSmoothedPing >= 0 )
		msgConnectRequest.set_ping_est_ms( m_connection.m_statsEndToEnd.m_ping.m_nSmoothedPing );
	*msgConnectRequest.mutable_cert() = m_connection.GetSignedCertLocal();
	*msgConnectRequest.mutable_crypt() = m_connection.GetSignedCryptLocal();

	// If the cert is generic, then we need to specify our identity
	if ( !m_connection.BCertHasIdentity() )
	{
		SteamNetworkingIdentityToProtobuf( IdentityLocal(), msgConnectRequest, identity_string, legacy_identity_binary, legacy_client_steam_id );
	}
	else
	{
		// Identity is in the cert.  But for old peers, set legacy field, if we are a SteamID
		if ( IdentityLocal().GetSteamID64() )
			msgConnectRequest.set_legacy_client_steam_id( IdentityLocal().GetSteamID64() );
	}

	// Send it
	SendMsg( k_ESteamNetworkingUDPMsg_ConnectRequest, msgConnectRequest );

	// Update retry bookkeeping, etc
	m_connection.SentEndToEndConnectRequest( usecNow );

	// They are supposed to reply with a timestamps, from which we can estimate the ping.
	// So this counts as a ping request
	m_connection.m_statsEndToEnd.TrackSentPingRequest( usecNow, false );
}