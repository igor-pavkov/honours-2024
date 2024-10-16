void CSteamNetworkListenSocketDirectUDP::Received_ConnectRequest( const CMsgSteamSockets_UDP_ConnectRequest &msg, const netadr_t &adrFrom, int cbPkt, SteamNetworkingMicroseconds usecNow )
{
	SteamDatagramErrMsg errMsg;

	// Make sure challenge was generated relatively recently
	uint16 nTimeThen = uint32( msg.challenge() );
	uint16 nElapsed = GetChallengeTime( usecNow ) - nTimeThen;
	if ( nElapsed > GetChallengeTime( 4*k_nMillion ) )
	{
		ReportBadPacket( "ConnectRequest", "Challenge too old." );
		return;
	}

	// Assuming we sent them this time value, re-create the challenge we would have sent them.
	if ( GenerateChallenge( nTimeThen, adrFrom ) != msg.challenge() )
	{
		ReportBadPacket( "ConnectRequest", "Incorrect challenge.  Could be spoofed." );
		return;
	}

	uint32 unClientConnectionID = msg.client_connection_id();
	if ( unClientConnectionID == 0 )
	{
		ReportBadPacket( "ConnectRequest", "Missing connection ID" );
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
			ReportBadPacket( "ConnectRequest", "Bad identity in cert.  %s", errMsg );
			return;
		}
		if ( r == 0 )
		{
			// No identity in the cert.  Check if they put it directly in the connect message
			bIdentityInCert = false;
			r = SteamNetworkingIdentityFromProtobuf( identityRemote, msg, identity_string, legacy_identity_binary, legacy_client_steam_id, errMsg );
			if ( r < 0 )
			{
				ReportBadPacket( "ConnectRequest", "Bad identity.  %s", errMsg );
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
		adrFrom.GetIPV6( addr.m_ipv6 );
		addr.m_port = adrFrom.GetPort();

		if ( identityRemote.IsLocalHost() )
		{
			if ( m_connectionConfig.m_IP_AllowWithoutAuth.Get() == 0 )
			{
				// Should we send an explicit rejection here?
				ReportBadPacket( "ConnectRequest", "Unauthenticated connections not allowed." );
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
				ReportBadPacket( "ConnectRequest", "Cannot use specific IP address." );
				return;
			}
		}
	}

	// Does this connection already exist?  (At a different address?)
	int h = m_mapChildConnections.Find( RemoteConnectionKey_t{ identityRemote, unClientConnectionID } );
	if ( h != m_mapChildConnections.InvalidIndex() )
	{
		CSteamNetworkConnectionBase *pOldConn = m_mapChildConnections[ h ];
		Assert( pOldConn->m_identityRemote == identityRemote );

		// NOTE: We cannot just destroy the object.  The API semantics
		// are that all connections, once accepted and made visible
		// to the API, must be closed by the application.
		ReportBadPacket( "ConnectRequest", "Rejecting connection request from %s at %s, connection ID %u.  That steamID/ConnectionID pair already has a connection [%s]\n",
			SteamNetworkingIdentityRender( identityRemote ).c_str(), CUtlNetAdrRender( adrFrom ).String(), unClientConnectionID, pOldConn->GetDescription()
		);

		CMsgSteamSockets_UDP_ConnectionClosed msgReply;
		msgReply.set_to_connection_id( unClientConnectionID );
		msgReply.set_reason_code( k_ESteamNetConnectionEnd_Misc_Generic );
		msgReply.set_debug( "A connection with that ID already exists." );
		SendPaddedMsg( k_ESteamNetworkingUDPMsg_ConnectionClosed, msgReply, adrFrom );
		return;
	}

	CSteamNetworkConnectionUDP *pConn = new CSteamNetworkConnectionUDP( m_pSteamNetworkingSocketsInterface );

	// OK, they have completed the handshake.  Accept the connection.
	if ( !pConn->BBeginAccept( this, adrFrom, m_pSock, identityRemote, unClientConnectionID, msg.cert(), msg.crypt(), errMsg ) )
	{
		SpewWarning( "Failed to accept connection from %s.  %s\n", CUtlNetAdrRender( adrFrom ).String(), errMsg );
		pConn->ConnectionDestroySelfNow();
		return;
	}

	pConn->m_statsEndToEnd.TrackRecvPacket( cbPkt, usecNow );

	// Did they send us a ping estimate?
	if ( msg.has_ping_est_ms() )
	{
		if ( msg.ping_est_ms() > 1500 )
		{
			SpewWarning( "[%s] Ignoring really large ping estimate %u in connect request", pConn->GetDescription(), msg.has_ping_est_ms() );
		}
		else
		{
			pConn->m_statsEndToEnd.m_ping.ReceivedPing( msg.ping_est_ms(), usecNow );
		}
	}

	// Save of timestamp that we will use to reply to them when the application
	// decides to accept the connection
	if ( msg.has_my_timestamp() )
	{
		pConn->m_ulHandshakeRemoteTimestamp = msg.my_timestamp();
		pConn->m_usecWhenReceivedHandshakeRemoteTimestamp = usecNow;
	}
}