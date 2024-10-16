bool CSteamNetworkListenSocketDirectUDP::BInit( const SteamNetworkingIPAddr &localAddr, int nOptions, const SteamNetworkingConfigValue_t *pOptions, SteamDatagramErrMsg &errMsg )
{
	Assert( m_pSock == nullptr );

	if ( localAddr.m_port == 0 )
	{
		V_strcpy_safe( errMsg, "Must specify local port." );
		return false;
	}

	// Set options, add us to the global table
	if ( !BInitListenSocketCommon( nOptions, pOptions, errMsg ) )
		return false;

	// Might we need to authenticate?
	int IP_AllowWithoutAuth = m_connectionConfig.m_IP_AllowWithoutAuth.Get();
	if ( IP_AllowWithoutAuth < 2 )
	{
		m_pSteamNetworkingSocketsInterface->AuthenticationNeeded();

		// If we know for sure that this can't ever work, then go ahead and fail now.
		#ifndef STEAMNETWORKINGSOCKETS_CAN_REQUEST_CERT
			if ( IP_AllowWithoutAuth == 0 )
			{
				V_strcpy_safe( errMsg, "No cert authority, must set IP_AllowWithoutAuth" );
				return false;
			}
		#endif
	}

	m_pSock = new CSharedSocket;
	if ( !m_pSock->BInit( localAddr, CRecvPacketCallback( ReceivedFromUnknownHost, this ), errMsg ) )
	{
		delete m_pSock;
		m_pSock = nullptr;
		return false;
	}

	CCrypto::GenerateRandomBlock( m_argbChallengeSecret, sizeof(m_argbChallengeSecret) );

	return true;
}