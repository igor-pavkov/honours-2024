bool CSteamNetworkConnectionlocalhostLoopback::APICreateSocketPair( CSteamNetworkingSockets *pSteamNetworkingSocketsInterface, CSteamNetworkConnectionlocalhostLoopback *pConn[2], const SteamNetworkingIdentity pIdentity[2] )
{
	SteamDatagramTransportLock::AssertHeldByCurrentThread();

	SteamDatagramErrMsg errMsg;

	pConn[1] = new CSteamNetworkConnectionlocalhostLoopback( pSteamNetworkingSocketsInterface, pIdentity[0] );
	pConn[0] = new CSteamNetworkConnectionlocalhostLoopback( pSteamNetworkingSocketsInterface, pIdentity[1] );
	if ( !pConn[0] || !pConn[1] )
	{
failed:
		pConn[0]->ConnectionDestroySelfNow(); pConn[0] = nullptr;
		pConn[1]->ConnectionDestroySelfNow(); pConn[1] = nullptr;
		return false;
	}

	// Don't post any state changes for these transitions.  We just want to immediately start in the
	// connected state
	++pConn[0]->m_nSupressStateChangeCallbacks;
	++pConn[1]->m_nSupressStateChangeCallbacks;

	CConnectionTransportUDP *pTransport[2] = {
		new CConnectionTransportUDP( *pConn[0] ),
		new CConnectionTransportUDP( *pConn[1] )
	};
	pConn[0]->m_pTransport = pTransport[0];
	pConn[1]->m_pTransport = pTransport[1];

	if ( !CConnectionTransportUDP::CreateLoopbackPair( pTransport ) )
		goto failed;

	SteamNetworkingMicroseconds usecNow = SteamNetworkingSockets_GetLocalTimestamp();

	// Initialize both connections
	for ( int i = 0 ; i < 2 ; ++i )
	{
		if ( !pConn[i]->BInitConnection( usecNow, 0, nullptr, errMsg ) )
		{
			AssertMsg1( false, "CSteamNetworkConnectionlocalhostLoopback::BInitConnection failed.  %s", errMsg );
			goto failed;
		}
	}

	// Tie the connections to each other, and mark them as connected
	for ( int i = 0 ; i < 2 ; ++i )
	{
		CSteamNetworkConnectionlocalhostLoopback *p = pConn[i];
		CSteamNetworkConnectionlocalhostLoopback *q = pConn[1-i];
		p->m_identityRemote = q->m_identityLocal;
		p->m_unConnectionIDRemote = q->m_unConnectionIDLocal;
		p->m_statsEndToEnd.m_usecTimeLastRecv = usecNow; // Act like we just now received something
		if ( !p->BRecvCryptoHandshake( q->m_msgSignedCertLocal, q->m_msgSignedCryptLocal, i==0 ) )
		{
			AssertMsg( false, "BRecvCryptoHandshake failed creating localhost socket pair" );
			goto failed;
		}
		if ( !p->BConnectionState_Connecting( usecNow, errMsg ) )
		{
			AssertMsg( false, "BConnectionState_Connecting failed creating loopback pipe socket pair.  %s", errMsg );
			goto failed;
		}
		p->ConnectionState_Connected( usecNow );
	}

	// Any further state changes are legit
	pConn[0]->m_nSupressStateChangeCallbacks = 0;
	pConn[1]->m_nSupressStateChangeCallbacks = 0;

	return true;
}