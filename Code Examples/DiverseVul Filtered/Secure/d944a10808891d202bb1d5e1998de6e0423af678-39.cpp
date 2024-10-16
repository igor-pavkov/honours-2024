void CConnectionTransportUDP::Received_ConnectOK( const CMsgSteamSockets_UDP_ConnectOK &msg, SteamNetworkingMicroseconds usecNow )
{
	SteamDatagramErrMsg errMsg;

	// We should only be getting this if we are the "client"
	if ( ListenSocket() )
	{
		ReportBadUDPPacketFromConnectionPeer( "ConnectOK", "Shouldn't be receiving this unless on accepted connections, only connections initiated locally." );
		return;
	}

	// Check connection ID to make sure they aren't spoofing and it's the same connection we think it is
	if ( msg.client_connection_id() != ConnectionIDLocal() )
	{
		ReportBadUDPPacketFromConnectionPeer( "ConnectOK", "Incorrect connection ID.  Message is stale or could be spoofed, ignoring." );
		return;
	}

	// Parse out identity from the cert
	SteamNetworkingIdentity identityRemote;
	bool bIdentityInCert = true;
	{
		// !SPEED! We are deserializing the cert here,
		// and then we are going to do it again below.
		// Should refactor to fix this.
		int r = SteamNetworkingIdentityFromSignedCert( identityRemote, msg.cert(), errMsg );
		if ( r < 0 )
		{
			ReportBadUDPPacketFromConnectionPeer( "ConnectRequest", "Bad identity in cert.  %s", errMsg );
			return;
		}
		if ( r == 0 )
		{
			// No identity in the cert.  Check if they put it directly in the connect message
			bIdentityInCert = false;
			r = SteamNetworkingIdentityFromProtobuf( identityRemote, msg, identity_string, legacy_identity_binary, legacy_server_steam_id, errMsg );
			if ( r < 0 )
			{
				ReportBadUDPPacketFromConnectionPeer( "ConnectRequest", "Bad identity.  %s", errMsg );
				return;
			}
			if ( r == 0 )
			{
				// If no identity was presented, it's the same as them saying they are "localhost"
				identityRemote.SetLocalHost();
			}
		}
	}
	Assert( !identityRemote.IsInvalid() );

	// Check if they are using an IP address as an identity (possibly the anonymous "localhost" identity)
	if ( identityRemote.m_eType == k_ESteamNetworkingIdentityType_IPAddress )
	{
		SteamNetworkingIPAddr addr;
		const netadr_t &adrFrom = m_pSocket->GetRemoteHostAddr();
		adrFrom.GetIPV6( addr.m_ipv6 );
		addr.m_port = adrFrom.GetPort();

		if ( identityRemote.IsLocalHost() )
		{
			if ( m_connection.m_connectionConfig.m_IP_AllowWithoutAuth.Get() == 0 )
			{
				// Should we send an explicit rejection here?
				ReportBadUDPPacketFromConnectionPeer( "ConnectOK", "Unauthenticated connections not allowed." );
				return;
			}

			// Set their identity to their real address (including port)
			identityRemote.SetIPAddr( addr );
		}
		else
		{
			// FIXME - Should the address be required to match?
			// If we are behind NAT, it won't.
			//if ( memcmp( addr.m_ipv6, identityRemote.m_ip.m_ipv6, sizeof(addr.m_ipv6) ) != 0
			//	|| ( identityRemote.m_ip.m_port != 0 && identityRemote.m_ip.m_port != addr.m_port ) ) // Allow 0 port in the identity to mean "any port"
			//{
			//	ReportBadPacket( "ConnectRequest", "Identity in request is %s, but packet is coming from %s." );
			//	return;
			//}

			// It's not really clear what the use case is here for
			// requesting a specific IP address as your identity,
			// and not using localhost.  If they have a cert, assume it's
			// meaningful.  Remember: the cert could be unsigned!  That
			// is a separate issue which will be handled later, whether
			// we want to allow that.
			if ( !bIdentityInCert )
			{
				// Should we send an explicit rejection here?
				ReportBadPacket( "ConnectOK", "Cannot use specific IP address." );
				return;
			}
		}
	}

	// Make sure they are still who we think they are
	if ( !m_connection.m_identityRemote.IsInvalid() && !( m_connection.m_identityRemote == identityRemote ) )
	{
		ReportBadUDPPacketFromConnectionPeer( "ConnectOK", "server_steam_id doesn't match who we expect to be connecting to!" );
		return;
	}

	// Update ping, if they replied a timestamp
	if ( msg.has_your_timestamp() )
	{
		SteamNetworkingMicroseconds usecElapsed = usecNow - (SteamNetworkingMicroseconds)msg.your_timestamp() - msg.delay_time_usec();
		if ( usecElapsed < 0 || usecElapsed > 2*k_nMillion )
		{
			SpewWarning( "Ignoring weird timestamp %llu in ConnectOK, current time is %llu, remote delay was %lld.\n", (unsigned long long)msg.your_timestamp(), usecNow, (long long)msg.delay_time_usec() );
		}
		else
		{
			int nPing = (usecElapsed + 500 ) / 1000;
			m_connection.m_statsEndToEnd.m_ping.ReceivedPing( nPing, usecNow );
		}
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

		case k_ESteamNetworkingConnectionState_Linger:
		case k_ESteamNetworkingConnectionState_Connected:
			// We already know we were able to establish the connection.
			// Just ignore this packet
			return;

		case k_ESteamNetworkingConnectionState_Connecting:
			break;
	}

	// Connection ID
	m_connection.m_unConnectionIDRemote = msg.server_connection_id();
	if ( ( m_connection.m_unConnectionIDRemote & 0xffff ) == 0 )
	{
		m_connection.ConnectionState_ProblemDetectedLocally( k_ESteamNetConnectionEnd_Remote_BadCrypt, "Didn't send valid connection ID" );
		return;
	}

	m_connection.m_identityRemote = identityRemote;

	// Check the certs, save keys, etc
	if ( !m_connection.BRecvCryptoHandshake( msg.cert(), msg.crypt(), false ) )
	{
		Assert( ConnectionState() == k_ESteamNetworkingConnectionState_ProblemDetectedLocally );
		ReportBadUDPPacketFromConnectionPeer( "ConnectOK", "Failed crypto init.  %s", m_connection.m_szEndDebug );
		return;
	}

	// Generic connection code will take it from here.
	m_connection.ConnectionState_Connected( usecNow );
}