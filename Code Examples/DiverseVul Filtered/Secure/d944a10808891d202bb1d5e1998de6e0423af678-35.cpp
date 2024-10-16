void CSteamNetworkListenSocketDirectUDP::Received_ConnectionClosed( const CMsgSteamSockets_UDP_ConnectionClosed &msg, const netadr_t &adrFrom, SteamNetworkingMicroseconds usecNow )
{
	// Send an ack.  Note that we require the inbound message to be padded
	// to a minimum size, and this reply is tiny, so we are not at a risk of
	// being used for reflection, even though the source address could be spoofed.
	CMsgSteamSockets_UDP_NoConnection msgReply;
	if ( msg.from_connection_id() )
		msgReply.set_to_connection_id( msg.from_connection_id() );
	if ( msg.to_connection_id() )
		msgReply.set_from_connection_id( msg.to_connection_id() );
	SendMsg( k_ESteamNetworkingUDPMsg_NoConnection, msgReply, adrFrom );
}