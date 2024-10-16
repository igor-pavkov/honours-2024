void CConnectionTransportUDPBase::SendNoConnection( uint32 unFromConnectionID, uint32 unToConnectionID )
{
	CMsgSteamSockets_UDP_NoConnection msg;
	if ( unFromConnectionID == 0 && unToConnectionID == 0 )
	{
		AssertMsg( false, "Can't send NoConnection, we need at least one of from/to connection ID!" );
		return;
	}
	if ( unFromConnectionID )
		msg.set_from_connection_id( unFromConnectionID );
	if ( unToConnectionID )
		msg.set_to_connection_id( unToConnectionID );
	SendMsg( k_ESteamNetworkingUDPMsg_NoConnection, msg );
}