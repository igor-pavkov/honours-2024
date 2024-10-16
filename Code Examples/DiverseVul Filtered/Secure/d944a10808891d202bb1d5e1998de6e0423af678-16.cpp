void CConnectionTransportUDPBase::SendConnectionClosedOrNoConnection()
{
	if ( ConnectionWireState() == k_ESteamNetworkingConnectionState_ClosedByPeer )
	{
		SendNoConnection( ConnectionIDLocal(), ConnectionIDRemote() );
	}
	else
	{
		CMsgSteamSockets_UDP_ConnectionClosed msg;
		msg.set_from_connection_id( ConnectionIDLocal() );

		if ( ConnectionIDRemote() )
			msg.set_to_connection_id( ConnectionIDRemote() );

		msg.set_reason_code( m_connection.m_eEndReason );
		if ( m_connection.m_szEndDebug[0] )
			msg.set_debug( m_connection.m_szEndDebug );
		SendPaddedMsg( k_ESteamNetworkingUDPMsg_ConnectionClosed, msg );
	}
}