void CConnectionTransportUDPBase::TrackSentStats( UDPSendPacketContext_t &ctx )
{

	// What effective flags will be received?
	bool bAllowDelayedReply = ( ctx.msg.flags() & ctx.msg.ACK_REQUEST_IMMEDIATE ) == 0;

	// Record that we sent stats and are waiting for peer to ack
	if ( ctx.msg.has_stats() )
	{
		m_connection.m_statsEndToEnd.TrackSentStats( ctx.msg.stats(), ctx.m_usecNow, bAllowDelayedReply );
	}
	else if ( ctx.msg.flags() & ctx.msg.ACK_REQUEST_E2E )
	{
		m_connection.m_statsEndToEnd.TrackSentMessageExpectingSeqNumAck( ctx.m_usecNow, bAllowDelayedReply );
	}

	// Spew appropriately
	SpewVerbose( "[%s] Sent UDP stats (%s):%s\n",
		ConnectionDescription(),
		ctx.m_pszReason,
		DescribeStatsContents( ctx.msg ).c_str()
	);
}