void UDPSendPacketContext_t::Populate( size_t cbHdrtReserve, EStatsReplyRequest eReplyRequested, CConnectionTransportUDPBase *pTransport )
{
	CSteamNetworkConnectionBase &connection = pTransport->m_connection;
	LinkStatsTracker<LinkStatsTrackerEndToEnd> &statsEndToEnd = connection.m_statsEndToEnd;

	int nFlags = 0;
	if ( connection.m_pTransport != pTransport )
		nFlags |= msg.NOT_PRIMARY_TRANSPORT_E2E;

	// What effective flags should we send
	int nReadyToSendTracer = 0;
	if ( eReplyRequested == k_EStatsReplyRequest_Immediate || statsEndToEnd.BNeedToSendPingImmediate( m_usecNow ) )
		nFlags |= msg.ACK_REQUEST_E2E | msg.ACK_REQUEST_IMMEDIATE;
	else if ( eReplyRequested == k_EStatsReplyRequest_DelayedOK || statsEndToEnd.BNeedToSendKeepalive( m_usecNow ) )
		nFlags |= msg.ACK_REQUEST_E2E;
	else
	{
		nReadyToSendTracer = statsEndToEnd.ReadyToSendTracerPing( m_usecNow );
		if ( nReadyToSendTracer > 1 )
			nFlags |= msg.ACK_REQUEST_E2E;
	}

	m_nFlags = nFlags;

	// Need to send any connection stats stats?
	m_nStatsNeed = statsEndToEnd.GetStatsSendNeed( m_usecNow );
	if ( m_nStatsNeed & k_nSendStats_Due )
	{
		statsEndToEnd.PopulateMessage( m_nStatsNeed, *msg.mutable_stats(), m_usecNow );

		if ( nReadyToSendTracer > 0 )
			m_nFlags |= msg.ACK_REQUEST_E2E;
	}

	// Populate flags now, based on what is implied from what we HAVE to send
	SlamFlagsAndCalcSize();
	CalcMaxEncryptedPayloadSize( cbHdrtReserve, &connection );

	// Would we like to try to send some additional stats, if there is room?
	if ( m_nStatsNeed & k_nSendStats_Ready )
	{
		if ( nReadyToSendTracer > 0 )
			m_nFlags |= msg.ACK_REQUEST_E2E;
		statsEndToEnd.PopulateMessage( m_nStatsNeed & k_nSendStats_Ready, *msg.mutable_stats(), m_usecNow );
		SlamFlagsAndCalcSize();
	}
}