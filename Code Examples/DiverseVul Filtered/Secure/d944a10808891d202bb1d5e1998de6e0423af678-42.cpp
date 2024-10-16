void CSteamNetworkListenSocketDirectUDP::Received_ChallengeRequest( const CMsgSteamSockets_UDP_ChallengeRequest &msg, const netadr_t &adrFrom, SteamNetworkingMicroseconds usecNow )
{
	if ( msg.connection_id() == 0 )
	{
		ReportBadPacket( "ChallengeRequest", "Missing connection_id." );
		return;
	}
	//CSteamID steamIDClient( uint64( msg.client_steam_id() ) );
	//if ( !steamIDClient.IsValid() )
	//{
	//	ReportBadPacket( "ChallengeRequest", "Missing/invalid SteamID.", cbPkt );
	//	return;
	//}

	// Get time value of challenge
	uint16 nTime = GetChallengeTime( usecNow );

	// Generate a challenge
	uint64 nChallenge = GenerateChallenge( nTime, adrFrom );

	// Send them a reply
	CMsgSteamSockets_UDP_ChallengeReply msgReply;
	msgReply.set_connection_id( msg.connection_id() );
	msgReply.set_challenge( nChallenge );
	msgReply.set_your_timestamp( msg.my_timestamp() );
	msgReply.set_protocol_version( k_nCurrentProtocolVersion );
	SendMsg( k_ESteamNetworkingUDPMsg_ChallengeReply, msgReply, adrFrom );
}