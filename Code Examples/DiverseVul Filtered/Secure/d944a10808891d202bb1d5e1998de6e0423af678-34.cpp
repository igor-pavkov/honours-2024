std::string DescribeStatsContents( const CMsgSteamSockets_UDP_Stats &msg )
{
	std::string sWhat;
	if ( msg.flags() & msg.ACK_REQUEST_E2E )
		sWhat += " request_ack";
	if ( msg.flags() & msg.ACK_REQUEST_IMMEDIATE )
		sWhat += " request_ack_immediate";
	if ( msg.flags() & msg.NOT_PRIMARY_TRANSPORT_E2E )
		sWhat += " backup_transport";
	if ( msg.stats().has_lifetime() )
		sWhat += " stats.life";
	if ( msg.stats().has_instantaneous() )
		sWhat += " stats.rate";
	return sWhat;
}