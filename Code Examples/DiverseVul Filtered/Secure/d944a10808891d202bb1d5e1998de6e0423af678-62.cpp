void CConnectionTransportUDPBase::RecvStats( const CMsgSteamSockets_UDP_Stats &msgStatsIn, SteamNetworkingMicroseconds usecNow )
{

	// Connection quality stats?
	if ( msgStatsIn.has_stats() )
		m_connection.m_statsEndToEnd.ProcessMessage( msgStatsIn.stats(), usecNow );

	// Spew appropriately
	SpewVerbose( "[%s] Recv UDP stats:%s\n",
		ConnectionDescription(),
		DescribeStatsContents( msgStatsIn ).c_str()
	);

	// Check if we need to reply, either now or later
	if ( m_connection.BStateIsActive() )
	{

		// Check for queuing outgoing acks
		if ( ( msgStatsIn.flags() & msgStatsIn.ACK_REQUEST_E2E ) || msgStatsIn.has_stats() )
		{
			bool bImmediate = ( msgStatsIn.flags() & msgStatsIn.ACK_REQUEST_IMMEDIATE ) != 0;
			m_connection.QueueEndToEndAck( bImmediate, usecNow );

			// Check if need to send an immediately reply, either because they
			// requested it, or because we are not the currently selected transport,
			// and we need to need to make sure the reply goes out using us
			if ( bImmediate || m_connection.m_pTransport != this )
			{
				SendEndToEndStatsMsg( k_EStatsReplyRequest_NothingToSend, usecNow, "AckStats" );
			}
		}
	}
}