CSteamNetworkListenSocketDirectUDP::CSteamNetworkListenSocketDirectUDP( CSteamNetworkingSockets *pSteamNetworkingSocketsInterface )
: CSteamNetworkListenSocketBase( pSteamNetworkingSocketsInterface )
{
	m_pSock = nullptr;
}