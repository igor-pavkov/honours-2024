void CConnectionTransportUDP::SendEndToEndConnectRequest( SteamNetworkingMicroseconds usecNow )
{
	Assert( !ListenSocket() );
	Assert( !m_connection.m_bConnectionInitiatedRemotely );
	Assert( ConnectionState() == k_ESteamNetworkingConnectionState_Connecting ); // Why else would we be doing this?
	Assert( ConnectionIDLocal() );

	CMsgSteamSockets_UDP_ChallengeRequest msg;
	msg.set_connection_id( ConnectionIDLocal() );
	//msg.set_client_steam_id( m_steamIDLocal.ConvertToUint64() );
	msg.set_my_timestamp( usecNow );
	msg.set_protocol_version( k_nCurrentProtocolVersion );

	// Send it, with padding
	SendPaddedMsg( k_ESteamNetworkingUDPMsg_ChallengeRequest, msg );

	// They are supposed to reply with a timestamps, from which we can estimate the ping.
	// So this counts as a ping request
	m_connection.m_statsEndToEnd.TrackSentPingRequest( usecNow, false );
}