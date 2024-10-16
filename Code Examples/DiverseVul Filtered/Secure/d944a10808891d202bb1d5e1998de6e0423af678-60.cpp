void CConnectionTransportUDPBase::Received_ConnectionClosed( const CMsgSteamSockets_UDP_ConnectionClosed &msg, SteamNetworkingMicroseconds usecNow )
{
	// Give them a reply to let them know we heard from them.  If it's the right connection ID,
	// then they probably aren't spoofing and it's critical that we give them an ack!
	//
	// If the wrong connection ID, then it could be an old connection so we'd like to send a reply
	// to let them know that they can stop telling us the connection is closed.
	// However, it could just be random garbage, so we need to protect ourselves from abuse,
	// so limit how many of these we send.
	bool bConnectionIDMatch =
		msg.to_connection_id() == ConnectionIDLocal()
		|| ( msg.to_connection_id() == 0 && msg.from_connection_id() && msg.from_connection_id() == m_connection.m_unConnectionIDRemote ); // they might not know our ID yet, if they are a client aborting the connection really early.
	if ( bConnectionIDMatch || BCheckGlobalSpamReplyRateLimit( usecNow ) )
	{
		// Send a reply, echoing exactly what they sent to us
		CMsgSteamSockets_UDP_NoConnection msgReply;
		if ( msg.to_connection_id() )
			msgReply.set_from_connection_id( msg.to_connection_id() );
		if ( msg.from_connection_id() )
			msgReply.set_to_connection_id( msg.from_connection_id() );
		SendMsg( k_ESteamNetworkingUDPMsg_NoConnection, msgReply );
	}

	// If incorrect connection ID, then that's all we'll do, since this packet actually
	// has nothing to do with current connection at all.
	if ( !bConnectionIDMatch )
		return;

	// Generic connection code will take it from here.
	m_connection.ConnectionState_ClosedByPeer( msg.reason_code(), msg.debug().c_str() );
}