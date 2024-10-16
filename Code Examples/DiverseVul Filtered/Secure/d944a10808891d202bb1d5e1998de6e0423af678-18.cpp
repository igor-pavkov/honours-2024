void CConnectionTransportUDP::TransportConnectionStateChanged( ESteamNetworkingConnectionState eOldState )
{
	CConnectionTransport::TransportConnectionStateChanged( eOldState );

	switch ( ConnectionState() )
	{
		case k_ESteamNetworkingConnectionState_FindingRoute: // not used for raw UDP
		default:
			Assert( false );
		case k_ESteamNetworkingConnectionState_None:
		case k_ESteamNetworkingConnectionState_Dead:
			return;

		case k_ESteamNetworkingConnectionState_FinWait:
		case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
			SendConnectionClosedOrNoConnection();
			break;

		case k_ESteamNetworkingConnectionState_Linger:
			break;

		case k_ESteamNetworkingConnectionState_Connecting:
		case k_ESteamNetworkingConnectionState_Connected:
		case k_ESteamNetworkingConnectionState_ClosedByPeer:
			break;
	}
}