void CConnectionTransportUDP::TransportPopulateConnectionInfo( SteamNetConnectionInfo_t &info ) const
{
	CConnectionTransport::TransportPopulateConnectionInfo( info );

	if ( m_pSocket )
	{
		const netadr_t &addr = m_pSocket->GetRemoteHostAddr();
		NetAdrToSteamNetworkingIPAddr( info.m_addrRemote, addr );
		if ( addr.IsLoopback() )
			info.m_eTransportKind = k_ESteamNetTransport_LocalHost;
		else if ( m_connection.m_statsEndToEnd.m_ping.m_nSmoothedPing <= 5 && IsRouteToAddressProbablyLocal( addr ) )
			info.m_eTransportKind = k_ESteamNetTransport_UDPProbablyLocal;
		else
			info.m_eTransportKind = k_ESteamNetTransport_UDP;
	}
}