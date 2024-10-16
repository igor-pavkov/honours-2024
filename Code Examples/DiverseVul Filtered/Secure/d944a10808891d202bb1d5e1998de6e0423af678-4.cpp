bool CSteamNetworkConnectionUDP::BInitConnect( const SteamNetworkingIPAddr &addressRemote, int nOptions, const SteamNetworkingConfigValue_t *pOptions, SteamDatagramErrMsg &errMsg )
{
	AssertMsg( !m_pTransport, "Trying to connect when we already have a socket?" );

	// We're initiating a connection, not being accepted on a listen socket
	Assert( !m_pParentListenSocket );
	Assert( !m_bConnectionInitiatedRemotely );

	netadr_t netadrRemote;
	SteamNetworkingIPAddrToNetAdr( netadrRemote, addressRemote );

	// We use identity validity to denote when our connection has been accepted,
	// so it's important that it be cleared.  (It should already be so.)
	Assert( m_identityRemote.IsInvalid() );
	m_identityRemote.Clear();

	// We should know our own identity, unless the app has said it's OK to go without this.
	if ( m_identityLocal.IsInvalid() ) // Specific identity hasn't already been set (by derived class, etc)
	{

		// Use identity from the interface, if we have one
		m_identityLocal = m_pSteamNetworkingSocketsInterface->InternalGetIdentity();
		if ( m_identityLocal.IsInvalid())
		{

			// We don't know who we are.  Should we attempt anonymous?
			if ( m_connectionConfig.m_IP_AllowWithoutAuth.Get() == 0 )
			{
				V_strcpy_safe( errMsg, "Unable to determine local identity, and auth required.  Not logged in?" );
				return false;
			}

			m_identityLocal.SetLocalHost();
		}
	}

	// Create transport.
	CConnectionTransportUDP *pTransport = new CConnectionTransportUDP( *this );
	if ( !pTransport->BConnect( netadrRemote, errMsg ) )
	{
		pTransport->TransportDestroySelfNow();
		return false;
	}
	m_pTransport = pTransport;

	// Let base class do some common initialization
	SteamNetworkingMicroseconds usecNow = SteamNetworkingSockets_GetLocalTimestamp();
	if ( !CSteamNetworkConnectionBase::BInitConnection( usecNow, nOptions, pOptions, errMsg ) )
	{
		DestroyTransport();
		return false;
	}

	// Start the connection state machine
	return BConnectionState_Connecting( usecNow, errMsg );
}